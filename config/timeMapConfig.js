/**
 * Time-based image mapping configuration
 * Maps hour ranges to corresponding character mood images
 */
export const TIME_MAP = [
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

export const FALLBACK_IMAGE = 'moods/anya1.jpeg';
export const IMAGE_BASE_PATH = 'moods/time/';
