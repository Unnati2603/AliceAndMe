export function getTimeBasedImagePath() {
    const now = new Date();
    const hours = now.getHours();
    console.log("Current Hour:", hours); // 🔍 Check current hour

    const timeMap = [
        { range: [1, 6], file: '1am6am.png' },
        { range: [6, 8], file: '6am8am.png' },
        { range: [8, 10], file: '8am10am.png' },
        { range: [10, 12], file: '10am12pm.png' },
        { range: [12, 13], file: '12pm1am.png' },
        { range: [13, 14], file: '1pm2pm.png' },
        { range: [14, 16], file: '2pm4pm.png' },
        { range: [16, 18], file: '4pm6pm.png' },
        { range: [18, 19], file: '6pm7pm.png' },
        { range: [19, 20], file: '7pm8pm.png' },
        { range: [20, 23], file: '8pm11pm.png' },
        { range: [23, 24], file: '11pm12am.png' },
        { range: [0, 1], file: '12am1am.png' },
    ];

    const entry = timeMap.find(t => hours >= t.range[0] && hours < t.range[1]);
    const selectedFile = entry ? `moods/time/${entry.file}` : 'moods/anya1.jpeg';
    // console.log("Full path:", window.location.origin + '/' + src);

    console.log("Selected File:", selectedFile); // 🔍 Confirm file being returned
    return selectedFile;
}
