/* File: components/hoverImageChanger.js */

/**
 * Sets up a delayed image change on hover for a given element.
 * @param {HTMLImageElement} imageElement - The <img> element to apply the behavior to.
 * @param {string[]} imagePaths - Array of image paths to choose from randomly.
 * @param {number} delay - Delay in milliseconds before changing the image (default 2000ms).
 */
export function setupDelayedImageChange(imageElement, imagePaths, delay = 2000) {
  let timerId = null;
  const originalSrc = imageElement.src;

  imageElement.addEventListener('mouseenter', () => {
    // Clear any existing timer
    if (timerId) clearTimeout(timerId);

    // Start a new timer to change the image after `delay`
    timerId = setTimeout(() => {
      // Pick a random image different from the current src
      let randomSrc;
      do {
        randomSrc = imagePaths[Math.floor(Math.random() * imagePaths.length)];
      } while (randomSrc === imageElement.src);

      imageElement.src = randomSrc;
      timerId = null;
    }, delay);
  });

  imageElement.addEventListener('mouseleave', () => {
    // Cancel the timer if the cursor leaves before delay
    if (timerId) clearTimeout(timerId);
    timerId = null;

    // Optionally revert to original on leave:
    // imageElement.src = originalSrc;
  });
}