// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */

#include "WebApi_maintenance.h"
#include "WebApi.h"
#include "WebApi_errors.h"
#include <AsyncJson.h>

void WebApiMaintenanceClass::init(AsyncWebServer* server)
{
    using std::placeholders::_1;

    _server = server;

    _server->on("/api/maintenance/reboot", HTTP_POST, std::bind(&WebApiMaintenanceClass::onRebootPost, this, _1));
}

void WebApiMaintenanceClass::loop()
{
}

void WebApiMaintenanceClass::onRebootPost(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }

    AsyncJsonResponse* response = new AsyncJsonResponse(false, MQTT_JSON_DOC_SIZE);
    JsonObject retMsg = response->getRoot();
    retMsg[F("type")] = F("warning");

    if (!request->hasParam("data", true)) {
        retMsg[F("message")] = F("No values found!");
        retMsg[F("code")] = WebApiError::GenericNoValueFound;
        response->setLength();
        request->send(response);
        return;
    }

    String json = request->getParam("data", true)->value();

    if (json.length() > MQTT_JSON_DOC_SIZE) {
        retMsg[F("message")] = F("Data too large!");
        retMsg[F("code")] = WebApiError::GenericDataTooLarge;
        response->setLength();
        request->send(response);
        return;
    }

    DynamicJsonDocument root(MQTT_JSON_DOC_SIZE);
    DeserializationError error = deserializeJson(root, json);

    if (error) {
        retMsg[F("message")] = F("Failed to parse data!");
        retMsg[F("code")] = WebApiError::GenericParseError;
        response->setLength();
        request->send(response);
        return;
    }

    if (!(root.containsKey("reboot"))) {
        retMsg[F("message")] = F("Values are missing!");
        retMsg[F("code")] = WebApiError::GenericValueMissing;
        response->setLength();
        request->send(response);
        return;
    }

    if (root[F("reboot")].as<bool>()) {
        retMsg[F("type")] = F("success");
        retMsg[F("message")] = F("Reboot triggered!");
        retMsg[F("code")] = WebApiError::MaintenanceRebootTriggered;

        response->setLength();
        request->send(response);
        yield();
        delay(1000);
        yield();
        ESP.restart();
    } else {
        retMsg[F("message")] = F("Reboot cancled!");
        retMsg[F("code")] = WebApiError::MaintenanceRebootCancled;

        response->setLength();
        request->send(response);
    }
}