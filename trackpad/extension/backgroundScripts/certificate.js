"use strict";

/**
 * Event called when browser first starts. Extension must start up storage and add startup tab
 */
// browser.runtime.onStartup.addListener();

/**
 * Event called when the extension is installed. Extension must start up storage and find any preexisting tabs
 * Assumption: No matter the reason of installation (first time or reload), there is not preexisting data stored
 * Observation: Local storage persists between reloads of the extension
 */
browser.runtime.onInstalled.addListener(() => {
    
});

/**
 * Event called when a tab is created
 */
// browser.tabs.onCreated.addListener();

/**
 * Event called when user switches between tabs
 */
// browser.tabs.onActivated.addListener();

/**
 * Event called when a tab is deleted
 */
// browser.tabs.onRemoved.addListener();

/**
 * Event called when HTTP request headers arrive
 */
// browser.webRequest.onHeadersReceived.addListener();