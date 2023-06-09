// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */
#include "WebApi_security.h"
#include "Configuration.h"
#include "WebApi.h"
#include "WebApi_errors.h"
#include "helper.h"
#include <AsyncJson.h>

void WebApiSecurityClass::init(AsyncWebServer* server)
{
    using std::placeholders::_1;

    _server = server;

    _server->on("/api/security/config", HTTP_GET, std::bind(&WebApiSecurityClass::onSecurityGet, this, _1));
    _server->on("/api/security/config", HTTP_POST, std::bind(&WebApiSecurityClass::onSecurityPost, this, _1));
    _server->on("/api/security/authenticate", HTTP_GET, std::bind(&WebApiSecurityClass::onAuthenticateGet, this, _1));
}

void WebApiSecurityClass::loop()
{
}

void WebApiSecurityClass::onSecurityGet(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }

    AsyncJsonResponse* response = new AsyncJsonResponse();
    JsonObject root = response->getRoot();
    const CONFIG_T& config = Configuration.get();

    root["password"] = config.Security_Password;
    root["allow_readonly"] = config.Security_AllowReadonly;

    response->setLength();
    request->send(response);
}

void WebApiSecurityClass::onSecurityPost(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }

    AsyncJsonResponse* response = new AsyncJsonResponse();
    JsonObject retMsg = response->getRoot();
    retMsg["type"] = "warning";

    if (!request->hasParam("data", true)) {
        retMsg["message"] = "No values found!";
        retMsg["code"] = WebApiError::GenericNoValueFound;
        response->setLength();
        request->send(response);
        return;
    }

    String json = request->getParam("data", true)->value();

    if (json.length() > 1024) {
        retMsg["message"] = "Data too large!";
        retMsg["code"] = WebApiError::GenericDataTooLarge;
        response->setLength();
        request->send(response);
        return;
    }

    DynamicJsonDocument root(1024);
    DeserializationError error = deserializeJson(root, json);

    if (error) {
        retMsg["message"] = "Failed to parse data!";
        retMsg["code"] = WebApiError::GenericParseError;
        response->setLength();
        request->send(response);
        return;
    }

    if (!root.containsKey("password")
        && root.containsKey("allow_readonly")) {
        retMsg["message"] = "Values are missing!";
        retMsg["code"] = WebApiError::GenericValueMissing;
        response->setLength();
        request->send(response);
        return;
    }

    if (root["password"].as<String>().length() < 8 || root["password"].as<String>().length() > WIFI_MAX_PASSWORD_STRLEN) {
        retMsg["message"] = "Password must between 8 and " STR(WIFI_MAX_PASSWORD_STRLEN) " characters long!";
        retMsg["code"] = WebApiError::SecurityPasswordLength;
        retMsg["param"]["max"] = WIFI_MAX_PASSWORD_STRLEN;
        response->setLength();
        request->send(response);
        return;
    }

    CONFIG_T& config = Configuration.get();
    strlcpy(config.Security_Password, root["password"].as<String>().c_str(), sizeof(config.Security_Password));
    config.Security_AllowReadonly = root["allow_readonly"].as<bool>();
    Configuration.write();

    retMsg["type"] = "success";
    retMsg["message"] = "Settings saved!";
    retMsg["code"] = WebApiError::GenericSuccess;

    response->setLength();
    request->send(response);
}

void WebApiSecurityClass::onAuthenticateGet(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }

    AsyncJsonResponse* response = new AsyncJsonResponse();
    JsonObject retMsg = response->getRoot();
    retMsg["type"] = "success";
    retMsg["message"] = "Authentication successful!";
    retMsg["code"] = WebApiError::SecurityAuthSuccess;

    response->setLength();
    request->send(response);
}