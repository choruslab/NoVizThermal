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
        browser.storage.local.set({[tab.id] : null})
    }
});

/**
 * Event called when user switches between tabs. Activate/deactivate arduino of depending on the active tab
 */
browser.tabs.onActivated.addListener(async (tab) => {
    browser.alarms.clear(`CHECK_ARDUINO_FOR_${tab.previousTabId}`)
    try {
        let results = await browser.storage.local.get(tab.tabId.toString());

        if (Object.keys(results).length > 0) {
            let activeTabInfo = results[tab.tabId];
        
            if (activeTabInfo !== null) {
                let notifyArduino = activeTabInfo.insecure === true && activeTabInfo.stopped === false;
                requestNonVisualCue(notifyArduino);
                
                if (notifyArduino) {
                    createAlarm(tab.tabId);
                }
            }
        }
    } 
    catch (error) {
        console.log(`[browser.tabs.onActivated] ${error}`);
    }

});

/**
 * Event called when a tab is deleted. Remove the tab entry from storage.
 */
browser.tabs.onRemoved.addListener((tabId, _) => {
    console.log(`Removing Tab: ${tabId}`);
    try {
        browser.storage.local.remove(tabId.toString());
    }
    catch (error) {
        console.log(`[browser.tabs.onRemoved] ${error}`);
    }
});

/**
 * Event called when HTTP request headers arrive. Get information about the website's HTTPS certificate
 */
browser.webRequest.onHeadersReceived.addListener( async (details) => {
    try {
        let httpsInfo = await browser.webRequest.getSecurityInfo(details.requestId, {});

        let insecureStatus = isCertificateInsecure(httpsInfo.state);

        browser.storage.local.set({
            [details.tabId] : {
                url : details.url,
                insecure : insecureStatus,
                stopped : !insecureStatus,
                requestId : details.requestId
            }
        });

        requestNonVisualCue(insecureStatus);

        if (insecureStatus) {
            createAlarm(details.tabId);
        }
    } 
    catch (error) {
        console.log(`[browser.webRequest.onHeadersReceived] ${error}`);
    }

},
// Trigger events for all the main pages (not their resources)
{urls: ["<all_urls>"], types: ["main_frame"]}, 
["blocking"]
);


browser.alarms.onAlarm.addListener(async (info) => {
    try {
        let result = await requestStoppedData();
        if (result) {
            let id = info.name.split("_").slice(-1)
            let tab_info = await browser.storage.local.get(id)
            tab_info[id]['stopped'] = result
            browser.storage.local.set(tab_info);
    
            browser.alarms.clear(info.name);
        }
    } catch (error) {
        console.log(`[browser.alarms.onAlarm] ${error}`)
    }
});

/**
 * Define whether HTTPS certificate is secure enough. State is a string given by the webRequest.getSecurityInfo() method 
 * https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/API/webRequest/getSecurityInfo
 * @param {String} state 
 */
function isCertificateInsecure(state) {
    // Weak means cipher is not strong enough
    // https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/API/webRequest/SecurityInfo#weaknessreasons
    if (state === "weak" || state === "insecure") {
        return true;
    } 

    return false;
}

/**
 * Send request a non-visual cue.
 * @param {Boolean} signal 
 */
function requestNonVisualCue(signal) {
    // TODO: Change testing URL to Arduino URL
    let resource = "http://localhost:3000/" + (signal ? 'H' : 'L');

    try {
        fetch(resource, {method : 'PUT'});
    }
    catch (error) {
        console.log(`[requestNonVisualCue()] ${error}`);
    }
}

/**
 * Get data about stopped status of the non-visual cue
 */
async function requestStoppedData() {
    let resource = 'http://localhost:3000/stopped'

    try {
        return (await fetch(resource)).json()
    } 
    catch (error) {
        console.log(`[requestStoppedData()] ${error}`)
    }
}

/**
 * Create alarm to check arduino status
 */
function createAlarm(tab_id) {
    const NAME = `CHECK_ARDUINO_FOR_${tab_id}`
    const TIME = 0.2  // in minutes

    browser.alarms.create(NAME, {
        periodInMinutes: TIME
    });
}