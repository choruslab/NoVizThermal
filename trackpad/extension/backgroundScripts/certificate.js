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
// browser.runtime.onInstalled.addListener(() => {
    // browser.tabs.query({}).then(val => console.log(val));
// });

/**
 * Event called when a tab is created
 */
browser.tabs.onCreated.addListener((tab) => {
    // Exclude tabs that do not host content (i.e., dev tools tabs)
    if (tab.id !== browser.tabs.TAB_ID_NONE) {
        browser.storage.local.set({[tab.id] : {
            url : "",
            insecure : "",
            stopped : undefined,
            requestId : undefined 
        }})
    }
});

/**
 * Event called when user switches between tabs
 */
// browser.tabs.onActivated.addListener();

/**
 * Event called when a tab is deleted
 */
// browser.tabs.onRemoved.addListener();

/**
 * Event called when HTTP request headers arrive. Get information about the website's HTTPS certificate
 */
browser.webRequest.onHeadersReceived.addListener( async (details) => {
    let httpsInfo = await browser.webRequest.getSecurityInfo(details.requestId);

    let securityStatus = isCertificateSecure(httpsInfo.state);

    browser.storage.local.set({
        [details.tabId] : {
            url : details.url,
            insecure : securityStatus,
            stopped : securityStatus,
            requestId : details.requestId
        }
    })

},
// Trigger events for all the main pages (not their resources)
{urls: ["<all_urls>"], types: ["main_frame"]}, 
["blocking"]
);

/**
 * Define whether HTTPS certificate is secure enough
 */
function isCertificateSecure(state) {
    // Weak means cipher is not strong enough
    // https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/API/webRequest/SecurityInfo#weaknessreasons
    if (state in ["weak", "insecure"]) {
        return false;
    } 

    return true;
}