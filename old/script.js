
function updateCharacterImage() {
    const hour = new Date().getHours();
    const characterImage = document.getElementById('character');

    if (hour >= 6 && hour < 12) {
        // Morning
        characterImage.src = 'moods/';
    } else if (hour >= 12 && hour < 18) {
        // Afternoon
        characterImage.src = 'assets/afternoon.png';
    } else {
        // Evening/Night
        characterImage.src = 'assets/evening.png';
    }
}

// Call function when the page loads
window.onload = updateCharacterImage;
