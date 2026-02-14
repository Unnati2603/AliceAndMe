import { TIME_MAP, FALLBACK_IMAGE, IMAGE_BASE_PATH } from '../config/timeMapConfig.js';

/**
 * Gets the image path based on the current time
 * @returns {string} The path to the character image for the current hour
 */
export function getTimeBasedImagePath() {
    const now = new Date();
    const hours = now.getHours();

    const entry = TIME_MAP.find(t => hours >= t.range[0] && hours < t.range[1]);
    const selectedFile = entry ? `${IMAGE_BASE_PATH}${entry.file}` : FALLBACK_IMAGE;

    return selectedFile;
}
