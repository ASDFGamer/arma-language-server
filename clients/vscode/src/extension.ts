"use strict";
// The module 'vscode' contains the VS Code extensibility API
// Import the module and reference it with the alias vscode in your code below

import * as vscode from "vscode";
import * as languageClient from "vscode-languageclient";
import * as path from "path";
import * as fs from "fs";

import * as OpenSelected from "./commands/open_selected"
import * as OpenRpt from "./commands/open_rpt"

// Defines the search path of your language server DLL. (.NET Core)
const languageServerPaths = [
    "../../server/Debug/sqfvm_language_server.exe",
]

let client: languageClient.LanguageClient | undefined;

async function activateLanguageServer(context: vscode.ExtensionContext) {
    // The server is implemented in an executable application.
    let serverModule: string | undefined;
    for (let p of languageServerPaths) {
        p = context.asAbsolutePath(p);
        console.log(p);
        try {
            await fs.promises.access(p);
            serverModule = p;
            break;
        } catch (err) {
            // Skip this path.
        }
    }
    if (!serverModule) throw new URIError("Cannot find the language server module.");
    let workPath = path.dirname(serverModule);
    console.log(`Use ${serverModule} as server module.`);
    console.log(`Work path: ${workPath}.`);


    // If the extension is launched in debug mode then the debug server options are used
    // Otherwise the run options are used
    let serverOptions: languageClient.ServerOptions = {
        run: { command: serverModule, args: [], options: { cwd: workPath } },
        debug: { command: serverModule, args: ["--debug"], options: { cwd: workPath } }
    }
    // Options to control the language client
    let clientOptions: languageClient.LanguageClientOptions = {
        // Register the server for plain text documents
        documentSelector: [{language: "sqf"}],
        synchronize: {
            // Synchronize the setting section 'languageServerExample' to the server
            configurationSection: "sqfVmLanguageServer",
            // Notify the server about file changes to '.clientrc files contain in the workspace
            fileEvents: [
                vscode.workspace.createFileSystemWatcher("**/.clientrc"),
                vscode.workspace.createFileSystemWatcher("**/.sqf"),
            ]
        },
    }

    // Create the language client and start the client.
    client = new languageClient.LanguageClient("sqfVmLanguageServer", "SQF-VM Language Server", serverOptions, clientOptions);
    let disposable = client.start();

    // Push the disposable to the context's subscriptions so that the 
    // client can be deactivated on extension deactivation
    context.subscriptions.push(disposable);
    OpenSelected.activate(context);
    OpenRpt.activate(context);
}

// this method is called when your extension is activated
// your extension is activated the very first time the command is executed
export async function activate(context: vscode.ExtensionContext) {
    console.log("SQF-VM Language Server extension is now activated.");

    await activateLanguageServer(context);

    /*
    // The command has been defined in the package.json file
    let disposable = vscode.commands.registerCommand("extension.sayHello", () => {
        // The code you place here will be executed every time your command is executed

        // Display a message box to the user
        vscode.window.showInformationMessage("Hello World!");
    });
    context.subscriptions.push(disposable);
    */
}

// this method is called when your extension is deactivated
export async function deactivate(): Promise<void> {
    client?.stop();
}