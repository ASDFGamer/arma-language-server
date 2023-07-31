
#ifndef SQFVM_LANGUAGE_SERVER_DATABASE_CONTEXT_HPP
#define SQFVM_LANGUAGE_SERVER_DATABASE_CONTEXT_HPP

#include <filesystem>
#include <utility>
#include <sqlite_orm/sqlite_orm.h>
#include "tables/t_file.h"
#include "tables/t_reference.h"
#include "tables/t_variable.h"
#include "orm_mappings.hpp"

namespace sqfvm::language_server::database {

    namespace internal {
        // Maps the database connection out
        inline auto create_storage(const std::string &path) {
            using namespace sqfvm::language_server::database::tables;
            using namespace sqlite_orm;
            auto storage = make_storage(path,
                                        make_table(t_file::table_name,
                                                   make_column("id_pk", &t_file::id_pk, primary_key().autoincrement()),
                                                   make_column("last_changed", &t_file::last_changed),
                                                   make_column("path", &t_file::path),
                                                   make_column("is_outdated", &t_file::is_outdated),
                                                   make_column("is_deleted", &t_file::is_deleted)),
                                        make_table(t_reference::table_name,
                                                   make_column("id_pk", &t_reference::id_pk,
                                                               primary_key().autoincrement()),
                                                   make_column("file_fk", &t_reference::file_fk),
                                                   make_column("variable_fk", &t_reference::variable_fk),
                                                   make_column("access", &t_reference::access),
                                                   make_column("line", &t_reference::line),
                                                   make_column("column", &t_reference::column),
                                                   make_column("offset", &t_reference::offset),
                                                   make_column("length", &t_reference::length),
                                                   make_column("types", &t_reference::types),
                                                   foreign_key(&t_reference::file_fk)
                                                           .references(&t_file::id_pk),
                                                   foreign_key(&t_reference::variable_fk)
                                                           .references(&t_variable::id_pk)),
                                        make_table(t_variable::table_name,
                                                   make_column("id_pk", &t_variable::id_pk,
                                                               primary_key().autoincrement()),
                                                   make_column("name", &t_variable::name),
                                                   make_column("scope", &t_variable::scope),
                                                   make_column("opt_file_fk", &t_variable::opt_file_fk),
                                                   foreign_key(&t_variable::opt_file_fk)
                                                           .references(&t_file::id_pk)),
                                        make_table(t_diagnostic::table_name,
                                                   make_column("id_pk", &t_diagnostic::id_pk,
                                                               primary_key().autoincrement()),
                                                   make_column("file_fk", &t_diagnostic::file_fk),
                                                   make_column("severity", &t_diagnostic::severity),
                                                   make_column("code", &t_diagnostic::code),
                                                   make_column("message", &t_diagnostic::message),
                                                   make_column("content", &t_diagnostic::content),
                                                   make_column("line", &t_diagnostic::line),
                                                   make_column("column", &t_diagnostic::column),
                                                   make_column("offset", &t_diagnostic::offset),
                                                   make_column("length", &t_diagnostic::length),
                                                   foreign_key(&t_diagnostic::file_fk)
                                                           .references(&t_file::id_pk)));
            return storage;
        }
    }
    class context {
        std::filesystem::path m_db_path;
        bool m_bad;
        using storage_t = decltype(internal::create_storage(std::string{}));
        storage_t m_storage;
        std::map<std::string, sqlite_orm::sync_schema_result> m_sync_result;
        std::string m_error;

    public:
        explicit context(const std::filesystem::path &db_path)
                : m_db_path(absolute(db_path)),
                  m_bad(false),
                  m_storage(internal::create_storage(m_db_path.string())) {
            try {
                if (!std::filesystem::exists(m_db_path)) {
                    auto parent_path = m_db_path.parent_path();
                    std::filesystem::create_directories(parent_path);
                }
                m_sync_result = m_storage.sync_schema(false);
            } catch (const std::exception &e) {
                m_error = e.what();
                m_bad = true;
            }
        }


        // The sync results as provided by the sqlite_orm sync_schema method
        [[nodiscard]] std::map<std::string, sqlite_orm::sync_schema_result>
        sync_result() const { return m_sync_result; }

        // Whether the connection to the database could not be established
        [[nodiscard]] bool bad() const { return m_bad; }

        // Whether the connection to the database could be established
        [[nodiscard]] bool good() const { return !m_bad; }

        // Whether the connection to the database could be established
        [[nodiscard]] std::string error() const { return m_error; }

        // Returns the storage object
        [[nodiscard]] storage_t &storage() { return m_storage; }

        // Returns the storage object
        [[nodiscard]] const storage_t &storage() const { return m_storage; }

        // Returns the path to the database
        [[nodiscard]] const std::filesystem::path &db_path() const { return m_db_path; }
    };
}

#endif //SQFVM_LANGUAGE_SERVER_DATABASE_CONTEXT_HPP
