
#ifndef SQFVM_LANGUAGE_SERVER_DATABASE_CONTEXT_HPP
#define SQFVM_LANGUAGE_SERVER_DATABASE_CONTEXT_HPP

#include <filesystem>
#include <utility>
#include <sqlite_orm/sqlite_orm.h>
#include "tables/t_code_action.h"
#include "tables/t_code_action_change.h"
#include "tables/t_diagnostic.h"
#include "tables/t_hover.h"
#include "tables/t_file.h"
#include "tables/t_file_history.h"
#include "tables/t_reference.h"
#include "tables/t_variable.h"
#include "orm_mappings.hpp"
#include "tables/t_file_include.h"

namespace sqfvm::language_server::database {

    namespace internal {
        struct t_db_generation {
            static constexpr const char *table_name = "tDbGeneration";
            static const int expected_generation = 11;
            int id_pk;
            int generation;
        };

        // Maps the database connection out
        inline auto create_storage(const std::string &path) {
            using namespace sqfvm::language_server::database::tables;
            using namespace sqlite_orm;
            auto storage = make_storage(
                    path,
                    make_table(t_db_generation::table_name,
                               make_column("id_pk", &t_db_generation::id_pk, primary_key().autoincrement()),
                               make_column("generation", &t_db_generation::generation)),
                    make_table(t_file::table_name,
                               make_column("id_pk", &t_file::id_pk, primary_key().autoincrement()),
                               make_column("last_changed", &t_file::last_changed),
                               make_column("path", &t_file::path),
                               make_column("is_ignored", &t_file::is_ignored),
                               make_column("is_outdated", &t_file::is_outdated),
                               make_column("is_deleted", &t_file::is_deleted)),
                    make_table(t_hover::table_name,
                               make_column("id_pk", &t_hover::id_pk, primary_key().autoincrement()),
                               make_column("file_fk", &t_hover::file_fk),
                               make_column("start_line", &t_hover::start_line),
                               make_column("start_column", &t_hover::start_column),
                               make_column("end_line", &t_hover::end_line),
                               make_column("end_column", &t_hover::end_column),
                               make_column("markdown", &t_hover::markdown),
                               foreign_key(&t_hover::file_fk).references(&t_file::id_pk)),
                    make_table(t_file_history::table_name,
                               make_column("id_pk", &t_file_history::id_pk, primary_key().autoincrement()),
                               make_column("file_fk", &t_file_history::file_fk),
                               make_column("content", &t_file_history::content),
                               make_column("is_external", &t_file_history::is_external),
                               make_column("time_stamp_created", &t_file_history::time_stamp_created),
                               foreign_key(&t_file_history::file_fk).references(&t_file::id_pk)),
                    make_table(t_file_include::table_name,
                               make_column("id_pk", &t_file_include::id_pk, primary_key().autoincrement()),
                               make_column("file_included_fk", &t_file_include::file_included_fk),
                               make_column("file_included_in_fk", &t_file_include::file_included_in_fk),
                               make_column("source_file_fk", &t_file_include::source_file_fk),
                               foreign_key(&t_file_include::file_included_fk).references(&t_file::id_pk),
                               foreign_key(&t_file_include::file_included_in_fk).references(&t_file::id_pk)),
                    make_table(t_code_action::table_name,
                               make_column("id_pk", &t_code_action::id_pk, primary_key().autoincrement()),
                               make_column("file_fk", &t_code_action::file_fk),
                               make_column("kind", &t_code_action::kind),
                               make_column("text", &t_code_action::text),
                               make_column("identifier", &t_code_action::identifier),
                               foreign_key(&t_code_action::file_fk).references(&t_file::id_pk)),
                    make_table(t_code_action_change::table_name,
                               make_column("id_pk", &t_code_action_change::id_pk, primary_key().autoincrement()),
                               make_column("code_action_fk", &t_code_action_change::code_action_fk),
                               make_column("operation", &t_code_action_change::operation),
                               make_column("old_path", &t_code_action_change::old_path),
                               make_column("path", &t_code_action_change::path),
                               make_column("start_line", &t_code_action_change::start_line),
                               make_column("start_column", &t_code_action_change::start_column),
                               make_column("end_line", &t_code_action_change::end_line),
                               make_column("end_column", &t_code_action_change::end_column),
                               make_column("content", &t_code_action_change::content),
                               foreign_key(&t_code_action_change::code_action_fk).references(&t_code_action::id_pk)),
                    make_table(t_reference::table_name,
                               make_column("id_pk", &t_reference::id_pk, primary_key().autoincrement()),
                               make_column("file_fk", &t_reference::file_fk),
                               make_column("source_file_fk", &t_reference::source_file_fk),
                               make_column("variable_fk", &t_reference::variable_fk),
                               make_column("access", &t_reference::access),
                               make_column("line", &t_reference::line),
                               make_column("column", &t_reference::column),
                               make_column("offset", &t_reference::offset),
                               make_column("length", &t_reference::length),
                               make_column("types", &t_reference::types),
                               make_column("is_declaration", &t_reference::is_declaration),
                               make_column("is_magic_variable", &t_reference::is_magic_variable),
                               foreign_key(&t_reference::file_fk).references(&t_file::id_pk),
                               foreign_key(&t_reference::variable_fk).references(&t_variable::id_pk)),
                    make_table(t_variable::table_name,
                               make_column("id_pk", &t_variable::id_pk, primary_key().autoincrement()),
                               make_column("variable_name", &t_variable::variable_name),
                               make_column("scope", &t_variable::scope),
                               make_column("opt_file_fk", &t_variable::opt_file_fk),
                               foreign_key(&t_variable::opt_file_fk).references(&t_file::id_pk)),
                    make_table(t_diagnostic::table_name,
                               make_column("id_pk", &t_diagnostic::id_pk, primary_key().autoincrement()),
                               make_column("file_fk", &t_diagnostic::file_fk),
                               make_column("source_file_fk", &t_diagnostic::source_file_fk),
                               make_column("severity", &t_diagnostic::severity),
                               make_column("code", &t_diagnostic::code),
                               make_column("message", &t_diagnostic::message),
                               make_column("content", &t_diagnostic::content),
                               make_column("line", &t_diagnostic::line),
                               make_column("column", &t_diagnostic::column),
                               make_column("offset", &t_diagnostic::offset),
                               make_column("length", &t_diagnostic::length),
                               make_column("is_suppressed", &t_diagnostic::is_suppressed),
                               foreign_key(&t_diagnostic::source_file_fk).references(&t_file::id_pk),
                               foreign_key(&t_diagnostic::file_fk).references(&t_file::id_pk)));
            return storage;
        }
    }
    class context {
    public:
        using storage_t = decltype(internal::create_storage(std::string{}));
    private:
        std::filesystem::path m_db_path;
        bool m_bad;
        storage_t m_storage;
        std::map<std::string, sqlite_orm::sync_schema_result> m_sync_result;
        std::string m_error;

        void handle_generation() {
            auto generations = m_storage.get_all<internal::t_db_generation>();
            auto generation = generations.empty() ? std::nullopt : std::make_optional(generations.front());
            if (!generation.has_value()) {
                db_clear();
                m_storage.insert(internal::t_db_generation{
                        .id_pk = 1,
                        .generation = internal::t_db_generation::expected_generation
                });
                return;
            }
            if (generation->generation != internal::t_db_generation::expected_generation) {
                m_storage.remove_all<internal::t_db_generation>();
                m_storage.insert(internal::t_db_generation{
                        .id_pk = 1,
                        .generation = internal::t_db_generation::expected_generation
                });
                db_clear();
            }
        }

    public:
        explicit context(const std::filesystem::path &db_path)
                : m_db_path(absolute(db_path)),
                  m_bad(true),
                  m_storage(internal::create_storage(m_db_path.string())) {
        }

        void migrate() {
            // Sadly, sync_schema may fail if the database is filled with data under certain circumstances,
            // preventing the handle_generation() call to be called ever, which is why this
            // wonky two-step system is mandatory.


            // Attempt generation migration. If this does not work, an exception is thrown
            // that can be ignored because we first have to sync the schema.
            try {
                auto parent_path = m_db_path.parent_path();
                if (!std::filesystem::exists(parent_path)) {
                    std::filesystem::create_directories(parent_path);
                }
                handle_generation();
            } catch (const std::exception &e) {
                /* empty */
            }

            // Sync the schema and migrate generation.
            // This is required to ensure the generation index is created and the database is too.
            try {
                auto parent_path = m_db_path.parent_path();
                if (!std::filesystem::exists(parent_path)) {
                    std::filesystem::create_directories(parent_path);
                }
                m_sync_result = m_storage.sync_schema(false);
                handle_generation();
                m_bad = false;
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
        [[nodiscard]] const storage_t &storage() const {
            // ToDo: Make this return multiple (named? always a new one?) sqlite databases and remove
            // analysis lock in language_server.hpp/-.cpp
            return m_storage;
        }

        // Returns the path to the database
        [[nodiscard]] const std::filesystem::path &db_path() const { return m_db_path; }

        std::optional<::sqfvm::language_server::database::tables::t_file> db_get_file_from_path(
                std::filesystem::path path,
                bool create_if_not_exists = false);

        void db_clear();

        struct operations {
            using errlogfnc_t = std::function<void(const std::string &)>;
            using success_t = bool;
            using abort_t = bool;

            [[nodiscard]] static success_t mark_all_files_as_deleted(
                    context &self,
                    const errlogfnc_t &fnc);

            [[nodiscard]] static std::pair<success_t, std::optional<tables::t_file>> find_file_by_path(
                    context &self,
                    const context::operations::errlogfnc_t &fnc,
                    std::filesystem::path fpath);

            [[nodiscard]] static std::pair<success_t, std::optional<tables::t_file>> find_file_by_id(
                    context &self,
                    const context::operations::errlogfnc_t &fnc,
                    uint64_t id);

            [[nodiscard]] static success_t update(
                    context &self,
                    const context::operations::errlogfnc_t &fnc,
                    tables::t_file file);

            [[nodiscard]] static success_t insert(
                    context &self,
                    const context::operations::errlogfnc_t &fnc,
                    tables::t_file file);

            [[nodiscard]] static success_t insert(
                    context &self,
                    const context::operations::errlogfnc_t &fnc,
                    tables::t_file_history file_history);

            [[nodiscard]] static success_t delete_files_flagged_with_is_deleted(
                    context &self,
                    const context::operations::errlogfnc_t &fnc);

            [[nodiscard]] static success_t for_each_file_not_outdated(
                    context &self,
                    const context::operations::errlogfnc_t &fnc,
                    const std::function<abort_t(const tables::t_file &)> &fnc2);

            [[nodiscard]] static success_t for_each_file_outdated_and_not_deleted(
                    context &self,
                    const context::operations::errlogfnc_t &fnc,
                    const std::function<abort_t(const tables::t_file &)> &fnc2);

            [[nodiscard]] static std::pair<success_t, std::vector<tables::t_reference>> find_all_references_by_file_and_line(
                    context &self,
                    const context::operations::errlogfnc_t &fnc,
                    const tables::t_file &file,
                    uint64_t line,
                    bool exclude_magical);

            [[nodiscard]] static std::pair<success_t, std::vector<tables::t_reference>> find_all_references_by_file_and_line(
                    context &self,
                    const context::operations::errlogfnc_t &fnc,
                    uint64_t file_id,
                    uint64_t line,
                    bool exclude_magical);

            [[nodiscard]] static std::pair<success_t, std::vector<tables::t_reference>> get_all_variables_of_variable(
                    context &self,
                    const context::operations::errlogfnc_t &fnc,
                    const tables::t_variable &variable);

            [[nodiscard]] static std::pair<success_t, std::vector<tables::t_reference>> get_all_variables_of_variable(
                    context &self,
                    const context::operations::errlogfnc_t &fnc,
                    uint64_t variable_id);
        };
    };
}

#endif //SQFVM_LANGUAGE_SERVER_DATABASE_CONTEXT_HPP
