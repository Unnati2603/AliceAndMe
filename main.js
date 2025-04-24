const { app, BrowserWindow } = require('electron');
const path = require('path');

//  require('electron'): Imports Electron’s core features.
//  app: Manages the app’s lifecycle (startup, close, etc.).
//  BrowserWindow: Creates the app window (where we load HTML/CSS/JS).
//  path: Helps with file paths (not used yet but useful later).

try {
    require('electron-reloader')(module);
} catch (err) {
    console.log("Reloading not supported:", err);
}

let mainWindow;
// global variable mainWindow to store the app window object.

app.whenReady().then(() => {
    mainWindow = new BrowserWindow({
        width: 500,
        height: 600,
        webPreferences: {
            nodeIntegration: true,
        },
        frame: false, // Borderless window for better look
        transparent: false, // Transparent background
        alwaysOnTop: true // Keeps the assistant visible
    });


    // By default, Electron disables Node.js in frontend for security reasons.

// 🔹 frame: false → Removes the title bar (so the assistant looks like a sticker).
// 🔹 transparent: true → Makes the background see-through (only character is visible).
// 🔹 alwaysOnTop: true → Keeps the assistant always above other windows.


    mainWindow.loadFile('index.html'); // Load frontend
});

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') app.quit();
});
// Closes the app when all windows are closed, except on MacOS (Darwin is Mac’s system name).