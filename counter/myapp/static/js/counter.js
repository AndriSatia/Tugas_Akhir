// Ambil mode yang dipilih dari localStorage
var selectedMode = localStorage.getItem('selectedMode');
// Ubah judul mode-title sesuai dengan mode yang dipilih
document.getElementById('mode-title').textContent = 'Mode ' + selectedMode;

var timerInterval; // Variabel untuk menyimpan interval waktu mundur
var counterInterval; // Variabel untuk menyimpan interval pengiriman data 'counter'
var counter = 0; // Variabel untuk menyimpan nilai counter
var wrong = 0;
var countdownTime = 60; // Waktu mundur dalam detik
var countdownElement = document.getElementById('countdown');
var timerRunning = false; // Flag untuk menandakan apakah timer sedang berjalan

// Tambahkan elemen audio untuk suara gerakan benar dan salah
var correctSound = new Audio('../static/sound/benar.wav');
var wrongSound = new Audio('../static/sound/salah.wav');

function startTimer() {
    if (!timerRunning) {
        timerRunning = true;
        clearInterval(counterInterval); // Hentikan interval pengambilan data 'counter' jika sebelumnya sudah berjalan
        clearInterval(timerInterval); // Hentikan interval waktu mundur jika sebelumnya sudah berjalan

        // Mulai interval untuk mengurangi waktu mundur setiap detik
        timerInterval = setInterval(function () {
            var minutes = Math.floor(countdownTime / 60);
            var seconds = countdownTime % 60;
            countdownElement.innerText = (minutes < 10 ? '0' : '') + minutes + ':' + (seconds < 10 ? '0' : '') + seconds;

            if (countdownTime <= 0) {
                clearInterval(timerInterval); // Hentikan interval jika waktu mundur habis
                stopReceivingData(); // Hentikan penerimaan data dari views.py
            } else {
                countdownTime--;
                if (countdownTime === 59) { // Mulai menerima data 'counter' dari views.py hanya saat waktu mundur dimulai
                    startReceivingData();
                }
            }
        }, 1000);
    }
}

function startReceivingData() {
    // Mulai mengambil data 'counter' dari views.py menggunakan AJAX setiap detik
    counterInterval = setInterval(function () {
        $.ajax({
            url: '/get-counter',
            type: 'GET',
            success: function (data) {
                if (data.data === 1) {
                    counter++;
                    $('#correctCounter').text(counter);
                    correctSound.play();
                }
                else if (data.data === 2) {
                    wrong++;
                    $('#wrongCounter').text(wrong);
                    // wrongSound.play();
                }
            },
            error: function (xhr, status, error) {
                console.error('Error:', error);
            }
        });
    }, 100);
}

function stopReceivingData() {
    clearInterval(counterInterval);
    clearInterval(timerInterval); // Hentikan interval waktu mundur jika tombol "Stop" ditekan
    timerRunning = false;
}

function restartTimer() {
    stopReceivingData(); // Hentikan semua interval
    counter = 0; // Reset nilai counter
    countdownTime = 60; // Reset waktu mundur ke 1 menit
    $('#correctCounter').text(counter); // Reset tampilan counter benar
    $('#wrongCounter').text(0); // Reset tampilan counter salah, jika diperlukan
    countdownElement.innerText = "01:00"; // Reset tampilan waktu mundur ke 1 menit
    timerRunning = false; // Set flag timerRunning ke false
}

function goToMode() {
    window.history.back();
}

function goToHistory() {
    window.location.href = '/history';
}

function saveData() {
    var name = prompt("Masukkan Nama:");
    if (name != null) {
        var countdownTimeText = $('#countdown').text().split(':');
        var remainingMinutes = parseInt(countdownTimeText[0]);
        var remainingSeconds = parseInt(countdownTimeText[1]);
        var elapsedSeconds = 60 - (remainingMinutes * 60 + remainingSeconds);

        var elapsedHours = Math.floor(elapsedSeconds / 3600);
        var remainingElapsedSeconds = elapsedSeconds % 3600;
        var elapsedMinutes = Math.floor(remainingElapsedSeconds / 60);
        var finalElapsedSeconds = remainingElapsedSeconds % 60;

        var elapsedTime = (elapsedHours < 10 ? '0' : '') + elapsedHours + ':' +
            (elapsedMinutes < 10 ? '0' : '') + elapsedMinutes + ':' +
            (finalElapsedSeconds < 10 ? '0' : '') + finalElapsedSeconds;

        var modeTitle = document.querySelector('.mode-title h1').innerText;
        var mode = modeTitle.includes('Push-up') ? 'Push-up' : 'Sit-up';

        $.ajax({
            type: 'POST',
            url: '/add-data/',  // URL to your Django view for adding data
            data: {
                'name': name,
                'correct_moves': $('#correctCounter').text(), // Get the correct moves count
                'incorrect_moves': $('#wrongCounter').text(), // Get the incorrect moves count
                'time': elapsedTime, // Set the time to the calculated elapsed time
                'mode': mode // Set the mode based on the title
            },
            success: function (response) {
                alert(response.message);
                goToHistory(); // Redirect to history page after successful save
            },
            error: function (xhr, status, error) {
                console.error('Error:', error);
            }
        });
    }
}

window.onload = function () {
    const urlParams = new URLSearchParams(window.location.search);
    const mode = urlParams.get('mode');
    if (mode) {
        document.getElementById('mode-title').textContent = 'Mode ' + mode;
    }
}
