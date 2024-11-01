/*
(c) Copyright 2023-2024, AirMettle, Inc.
All rights reserved
*/

#include "1d.hpp"
#include <crow/http_response.h>
#include <crow/mustache.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define CROW_STATIC_DIRECTORY "/usr/local/share/hmdh_ui/static"
#include "crow.h"
#include "crow/middlewares/cookie_parser.h"
#include "nlohmann/json.hpp"

#include "1d.hpp"
#include "2d.hpp"
#include "FloatingPointTrie/1-D.h"
#include "TrieSerialization/1-D.h"

bool file_exists(const std::string &name) {
  std::ifstream f(name.c_str());
  return f.good();
}

std::vector<char> readFile(std::string file_path) {
  std::ifstream file(file_path, std::ios::binary); // Open file in binary mode
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + file_path);
  }

  // Get file size
  file.seekg(0, std::ios::end);
  std::streamsize file_size = file.tellg();
  file.seekg(0, std::ios::beg);

  // Read file into vector
  std::vector<char> buffer(file_size);
  if (!file.read(buffer.data(), file_size)) {
    throw std::runtime_error("Error reading file: " + file_path);
  }

  return buffer;
}

int main(int argc, char *argv[]) {
  // crow::SimpleApp app;
  crow::App<crow::CookieParser> app;

  crow::mustache::set_global_base("/usr/local/share/hmdh_ui/templates/");

  // /usr/share/fph_deserialize/static/

  std::vector<char> buffer;
  trie_header header = {};
  size_t endOfHeaderOffset = 0;

  for (int argIdx = 1; argIdx < argc; ++argIdx) {
    std::string arg = argv[argIdx];
    if (arg == "-b" || arg == "--buffer") {
      if (argIdx + 1 >= argc) {
        std::cout << "Buffer path not provided" << std::endl;
        std::cout << "Continuing with default launch..." << std::endl;
      } else {
        std::string bufferPath = argv[argIdx + 1];
        std::cout << "Buffer path: " << bufferPath << std::endl;
        buffer = readFile(bufferPath);
        std::cout << "Buffer size: " << buffer.size() << std::endl;
        argIdx++;
      }
    }
  }

  CROW_ROUTE(app, "/")
  ([]() {
    auto page = crow::mustache::load_text("index.html");
    return page;
  });

  CROW_ROUTE(app, "/error/")
  ([]() {
    auto page = crow::mustache::load_text("error.html");
    return page;
  });

  CROW_ROUTE(app, "/upload/")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        crow::response res;

        crow::multipart::message_view file_message(req);
        for (const auto &part : file_message.part_map) {
          const auto &part_name = part.first;
          const auto &part_value = part.second;
          std::cout << "Part: " << part_name << '\n';
          if ("file" == part_name) {
            // Extract the file name
            auto headers_it = part_value.headers.find("Content-Disposition");
            if (headers_it == part_value.headers.end()) {
              std::cout << "No Content-Disposition found";
              res.redirect(
                  "/error/"); // Call the redirect function on the response
                              // objectwith the location specific to the config
              return res;
            }
            auto params_it = headers_it->second.params.find("filename");
            if (params_it == headers_it->second.params.end()) {
              std::cout << "Part with name \"file\" should have a file";
              res.redirect(
                  "/error/"); // Call the redirect function on the response
                              // objectwith the location specific to the config
              return res;
            }
            const std::string outfile_name{params_it->second};

            for (const auto &part_header : part_value.headers) {
              const auto &part_header_name = part_header.first;
              const auto &part_header_val = part_header.second;
              std::cout << "Header: " << part_header_name << '='
                        << part_header_val.value;
              for (const auto &param : part_header_val.params) {
                const auto &param_key = param.first;
                const auto &param_val = param.second;
                std::cout << " Param: " << param_key << ',' << param_val;
              }
            }

            std::cout << " Buffer size: " << buffer.size() << '\n';
            buffer.resize(part_value.body.size());
            std::copy(part_value.body.begin(), part_value.body.end(),
                      buffer.begin());
            std::cout << " Buffer size: " << buffer.size() << '\n';
          } else {
            std::cout << " Value: " << part_value.body << '\n';
          }
        }

        return crow::response(200);
      });

  CROW_ROUTE(app, "/config/").methods(crow::HTTPMethod::GET)([&]() {
    // Deserialize the header and find out the trie type. Set dim accordingly
    size_t offset = 0;
    header = deserializeTrieHeader(buffer, offset);
    endOfHeaderOffset = offset;

    std::string dim = std::string(header.type);

    std::cout << "Working on config: " << dim << std::endl;

    if (dim.compare("1-D") != 0 && dim.compare("2-D") != 0) {
      return crow::mustache::load_text("error.html");
    }

    if (dim.compare("1-D") == 0) {
      auto page = crow::mustache::load_text("index1d.html");
      return page;
    }

    if (dim.compare("2-D") == 0) {
      auto page = crow::mustache::load_text("index2d.html");
      return page;
    }

    return crow::mustache::load_text("error.html");
  });

  CROW_ROUTE(app, "/process/").methods(crow::HTTPMethod::GET)([&]() {
    switch (header.type[0] - '0') {
    case 1: {
      if (header.node_width == 8) {
        nlohmann::json payload_top_level = nlohmann::json::array();

        nlohmann::json specialCntNode;
        specialCntNode["NaN"] = header.nan_count;
        specialCntNode["-Inf"] = header.neg_inf_count;
        specialCntNode["+Inf"] = header.pos_inf_count;
        specialCntNode["Zero"] = header.neg_zero_count + header.pos_zero_count;

        payload_top_level.push_back(specialCntNode);

        nlohmann::json jsonNode =
            toJsonFlattendTLE1DxF(buffer, endOfHeaderOffset);
        payload_top_level.push_back(jsonNode);

        return crow::response(payload_top_level.dump(4));
      } else {
        return crow::response("Invalid node width");
      }
    }
    case 2: {
      if (header.node_width == 8) {
        nlohmann::json jsonNode =
            toJsonFlattendTLE2x8(buffer, endOfHeaderOffset);
        return crow::response(jsonNode.dump(4));
      } else {
        return crow::response("Invalid node width");
      }
    }
    default:
      return crow::response("Invalid trie type");
    }
  });

  CROW_ROUTE(app, "/drill/")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        auto jsonBody = crow::json::load(req.body);
        if (!jsonBody) {
          return crow::response(400, "Invalid JSON");
        }

        switch (header.type[0] - '0') {
        case 1: {
          int top_8_bits = jsonBody["top_8"].i();

          std::cout << "top_8_bits: " << top_8_bits << std::endl;
          if (header.node_width == 8) {
            nlohmann::json jsonNode =
                drill1DxF(buffer, endOfHeaderOffset, top_8_bits);
            return crow::response(jsonNode.dump(4));
          } else {
            return crow::response("Invalid node width");
          }
        }
        case 2: {
          int x = jsonBody["x"].i();
          int y = jsonBody["y"].i();

          std::cout << "X: " << x << " Y: " << y << std::endl;
          if (header.node_width == 8) {
            nlohmann::json jsonNode =
                drill2D2x8(buffer, endOfHeaderOffset, x, y);
            return crow::response(jsonNode.dump(4));
          } else {
            return crow::response("Invalid node width");
          }
        }
        default:
          return crow::response("Invalid trie type");
        }
      });

  app.port(18080).multithreaded().run();

  return EXIT_SUCCESS;
}