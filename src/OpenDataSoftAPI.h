#ifndef OPENDATASOFT_API_H
#define OPENDATASOFT_API_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class OpenDataSoftAPI {
private:
    std::string api_base = "https://daten.sg.ch/api/explore/v2.1";
    http_client_config client_config;
    
    http_request create_request(const std::string& endpoint, const std::string& method) {
        http_request request;
        
        if (method == "GET") {
            request.set_method(methods::GET);
        } else if (method == "POST") {
            request.set_method(methods::POST);
        } else if (method == "PUT") {
            request.set_method(methods::PUT);
        } else if (method == "DEL") {
            request.set_method(methods::DEL);
        }
        
        request.set_request_uri(utility::conversions::to_string_t(endpoint));
        
        // Set headers
        request.headers().add(U("Host"), U("daten.sg.ch"));
        request.headers().add(U("Content-Type"), U("application/json"));
        request.headers().add(U("User-Agent"), U("Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
        
        return request;
    }
    
    std::string build_query_params(const std::map<std::string, std::string>& params) {
        if (params.empty()) return "";
        
        std::string query = "?";
        bool first = true;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                if (!first) query += "&";
                auto encoded_value = web::uri::encode_data_string(utility::conversions::to_string_t(param.second));
                query += param.first + "=" + utility::conversions::to_utf8string(encoded_value);
                first = false;
            }
        }
        return query;
    }
    
    pplx::task<json::value> make_api_call(const std::string& endpoint, const std::string& method) {
        http_client client(utility::conversions::to_string_t(api_base), client_config);
        auto request = create_request(endpoint, method);

        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: ") + utility::conversions::to_string_t(std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: ") + utility::conversions::to_string_t(e.what()));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }

public:
    OpenDataSoftAPI() {
        client_config.set_validate_certificates(false);
    }

    // Catalog endpoints
    pplx::task<json::value> get_catalog_datasets(
        const std::string& select = "",
        const std::string& where = "",
        const std::string& order_by = "",
        int limit = 10,
        int offset = 0,
        const std::string& refine = "",
        const std::string& exclude = "",
        const std::string& lang = "",
        const std::string& timezone = "",
        const std::string& group_by = "",
        bool include_links = false,
        bool include_app_metas = false) {
        
        std::map<std::string, std::string> params;
        
        if (!select.empty()) params["select"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(select)));
        if (!where.empty()) params["where"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(where)));
        if (!order_by.empty()) params["order_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(order_by)));
        if (limit != 10) params["limit"] = std::to_string(limit);
        if (offset != 0) params["offset"] = std::to_string(offset);
        if (!refine.empty()) params["refine"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(refine)));
        if (!exclude.empty()) params["exclude"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(exclude)));
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!timezone.empty()) params["timezone"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(timezone)));
        if (!group_by.empty()) params["group_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(group_by)));
        if (include_links) params["include_links"] = "true";
        if (include_app_metas) params["include_app_metas"] = "true";
        
        return make_api_call("/catalog/datasets" + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_catalog_exports() {
        return make_api_call("/catalog/exports", "GET");
    }

    pplx::task<json::value> export_catalog(
        const std::string& format,
        const std::string& select = "",
        const std::string& where = "",
        const std::string& order_by = "",
        const std::string& group_by = "",
        int limit = -1,
        int offset = 0,
        const std::string& refine = "",
        const std::string& exclude = "",
        const std::string& lang = "",
        const std::string& timezone = "") {
        
        std::map<std::string, std::string> params;
        
        if (!select.empty()) params["select"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(select)));
        if (!where.empty()) params["where"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(where)));
        if (!order_by.empty()) params["order_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(order_by)));
        if (!group_by.empty()) params["group_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(group_by)));
        if (limit != -1) params["limit"] = std::to_string(limit);
        if (offset != 0) params["offset"] = std::to_string(offset);
        if (!refine.empty()) params["refine"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(refine)));
        if (!exclude.empty()) params["exclude"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(exclude)));
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!timezone.empty()) params["timezone"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(timezone)));
        
        return make_api_call("/catalog/exports/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(format))) + build_query_params(params), "GET");
    }

    pplx::task<json::value> export_catalog_csv(
        const std::string& select = "",
        const std::string& where = "",
        const std::string& order_by = "",
        const std::string& group_by = "",
        int limit = -1,
        int offset = 0,
        const std::string& refine = "",
        const std::string& exclude = "",
        const std::string& lang = "",
        const std::string& timezone = "",
        const std::string& delimiter = ";",
        const std::string& list_separator = ",",
        bool quote_all = false,
        bool with_bom = true) {
        
        std::map<std::string, std::string> params;
        
        // Standard parameters
        if (!select.empty()) params["select"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(select)));
        if (!where.empty()) params["where"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(where)));
        if (!order_by.empty()) params["order_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(order_by)));
        if (!group_by.empty()) params["group_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(group_by)));
        if (limit != -1) params["limit"] = std::to_string(limit);
        if (offset != 0) params["offset"] = std::to_string(offset);
        if (!refine.empty()) params["refine"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(refine)));
        if (!exclude.empty()) params["exclude"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(exclude)));
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!timezone.empty()) params["timezone"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(timezone)));
        
        // CSV-specific parameters
        if (delimiter != ";") params["delimiter"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(delimiter)));
        if (list_separator != ",") params["list_separator"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(list_separator)));
        if (quote_all) params["quote_all"] = "true";
        if (!with_bom) params["with_bom"] = "false";
        
        return make_api_call("/catalog/exports/csv" + build_query_params(params), "GET");
    }

    pplx::task<json::value> export_catalog_dcat(
        const std::string& dcat_ap_format,
        const std::string& include_exports = "",
        bool use_labels_in_exports = true) {
        
        std::map<std::string, std::string> params;
        
        if (!include_exports.empty()) params["include_exports"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(include_exports)));
        if (!use_labels_in_exports) params["use_labels_in_exports"] = "false";
        
        return make_api_call("/catalog/exports/dcat" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(dcat_ap_format))) + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_catalog_facets(
        const std::string& facet = "",
        const std::string& refine = "",
        const std::string& exclude = "",
        const std::string& where = "",
        const std::string& timezone = "") {
        
        std::map<std::string, std::string> params;
        
        if (!facet.empty()) params["facet"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(facet)));
        if (!refine.empty()) params["refine"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(refine)));
        if (!exclude.empty()) params["exclude"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(exclude)));
        if (!where.empty()) params["where"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(where)));
        if (!timezone.empty()) params["timezone"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(timezone)));
        
        return make_api_call("/catalog/facets" + build_query_params(params), "GET");
    }

    // Dataset endpoints
    pplx::task<json::value> get_dataset_info(
        const std::string& dataset_id,
        const std::string& select = "",
        const std::string& lang = "",
        const std::string& timezone = "",
        bool include_links = false,
        bool include_app_metas = false) {
        
        std::map<std::string, std::string> params;
        
        if (!select.empty()) params["select"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(select)));
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!timezone.empty()) params["timezone"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(timezone)));
        if (include_links) params["include_links"] = "true";
        if (include_app_metas) params["include_app_metas"] = "true";
        
        return make_api_call("/catalog/datasets/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(dataset_id))) + build_query_params(params), "GET");
    }

    pplx::task<json::value> query_dataset_records(
        const std::string& dataset_id,
        const std::string& select = "",
        const std::string& where = "",
        const std::string& group_by = "",
        const std::string& order_by = "",
        int limit = 10,
        int offset = 0,
        const std::string& refine = "",
        const std::string& exclude = "",
        const std::string& lang = "",
        const std::string& timezone = "",
        bool include_links = false,
        bool include_app_metas = false) {
        
        std::map<std::string, std::string> params;
        
        if (!select.empty()) params["select"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(select)));
        if (!where.empty()) params["where"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(where)));
        if (!group_by.empty()) params["group_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(group_by)));
        if (!order_by.empty()) params["order_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(order_by)));
        if (limit != 10) params["limit"] = std::to_string(limit);
        if (offset != 0) params["offset"] = std::to_string(offset);
        if (!refine.empty()) params["refine"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(refine)));
        if (!exclude.empty()) params["exclude"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(exclude)));
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!timezone.empty()) params["timezone"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(timezone)));
        if (include_links) params["include_links"] = "true";
        if (include_app_metas) params["include_app_metas"] = "true";
        
        return make_api_call("/catalog/datasets/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(dataset_id))) + "/records" + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_dataset_exports(const std::string& dataset_id) {
        return make_api_call("/catalog/datasets/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(dataset_id))) + "/exports", "GET");
    }

    pplx::task<json::value> export_dataset(
        const std::string& dataset_id,
        const std::string& format,
        const std::string& select = "",
        const std::string& where = "",
        const std::string& order_by = "",
        const std::string& group_by = "",
        int limit = -1,
        const std::string& refine = "",
        const std::string& exclude = "",
        const std::string& lang = "",
        const std::string& timezone = "",
        bool use_labels = false,
        bool compressed = false,
        int epsg = 4326) {
        
        std::map<std::string, std::string> params;
        
        if (!select.empty()) params["select"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(select)));
        if (!where.empty()) params["where"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(where)));
        if (!order_by.empty()) params["order_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(order_by)));
        if (!group_by.empty()) params["group_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(group_by)));
        if (limit != -1) params["limit"] = std::to_string(limit);
        if (!refine.empty()) params["refine"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(refine)));
        if (!exclude.empty()) params["exclude"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(exclude)));
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!timezone.empty()) params["timezone"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(timezone)));
        if (use_labels) params["use_labels"] = "true";
        if (compressed) params["compressed"] = "true";
        if (epsg != 4326) params["epsg"] = std::to_string(epsg);
        
        return make_api_call("/catalog/datasets/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(dataset_id))) + "/exports/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(format))) + build_query_params(params), "GET");
    }

    pplx::task<json::value> export_dataset_csv(
        const std::string& dataset_id,
        const std::string& select = "",
        const std::string& where = "",
        const std::string& order_by = "",
        const std::string& group_by = "",
        int limit = -1,
        const std::string& refine = "",
        const std::string& exclude = "",
        const std::string& lang = "",
        const std::string& timezone = "",
        const std::string& delimiter = ";",
        const std::string& list_separator = ",",
        bool quote_all = false,
        bool with_bom = true) {
        
        std::map<std::string, std::string> params;
        
        // Standard parameters
        if (!select.empty()) params["select"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(select)));
        if (!where.empty()) params["where"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(where)));
        if (!order_by.empty()) params["order_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(order_by)));
        if (!group_by.empty()) params["group_by"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(group_by)));
        if (limit != -1) params["limit"] = std::to_string(limit);
        if (!refine.empty()) params["refine"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(refine)));
        if (!exclude.empty()) params["exclude"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(exclude)));
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!timezone.empty()) params["timezone"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(timezone)));
        
        // CSV-specific parameters
        if (delimiter != ";") params["delimiter"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(delimiter)));
        if (list_separator != ",") params["list_separator"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(list_separator)));
        if (quote_all) params["quote_all"] = "true";
        if (!with_bom) params["with_bom"] = "false";
        
        return make_api_call("/catalog/datasets/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(dataset_id))) + "/exports/csv" + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_dataset_facets(
        const std::string& dataset_id,
        const std::string& where = "",
        const std::string& refine = "",
        const std::string& exclude = "",
        const std::string& facet = "",
        const std::string& lang = "",
        const std::string& timezone = "") {
        
        std::map<std::string, std::string> params;
        
        if (!where.empty()) params["where"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(where)));
        if (!refine.empty()) params["refine"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(refine)));
        if (!exclude.empty()) params["exclude"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(exclude)));
        if (!facet.empty()) params["facet"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(facet)));
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!timezone.empty()) params["timezone"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(timezone)));
        
        return make_api_call("/catalog/datasets/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(dataset_id))) + "/facets" + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_dataset_attachments(const std::string& dataset_id) {
        return make_api_call("/catalog/datasets/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(dataset_id))) + "/attachments", "GET");
    }

    pplx::task<json::value> get_dataset_record(
        const std::string& dataset_id,
        const std::string& record_id,
        const std::string& select = "",
        const std::string& lang = "",
        const std::string& timezone = "") {
        
        std::map<std::string, std::string> params;
        
        if (!select.empty()) params["select"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(select)));
        if (!lang.empty()) params["lang"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(lang)));
        if (!timezone.empty()) params["timezone"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(timezone)));
        
        return make_api_call("/catalog/datasets/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(dataset_id))) + "/records/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(record_id))) + build_query_params(params), "GET");
    }
};

#endif
