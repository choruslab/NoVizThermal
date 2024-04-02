"use strict";

browser.runtime.onStartup.addListener();

browser.runtime.onInstalled.addListener();

browser.tabs.onCreated.addListener();

browser.tabs.onActivated.addListener();

browser.tabs.onRemoved.addListener();

browser.webRequest.onHeadersReceived.addListener();