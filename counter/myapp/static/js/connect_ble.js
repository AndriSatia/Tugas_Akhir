function goToHistory() {
    window.location.href = '/history';
  }

  function connectBLE() {
    $.ajax({
      url: '/connect-ble?start=1',
      type: 'GET',
      success: function (data) {
        if (data.connected) {
          $('#connectionStatus').text('Status: Connected');
          $('#nextBtn').show(); // Tampilkan tombol Next setelah koneksi berhasil
        } else {
          $('#connectionStatus').text('Status: Connection Failed');
        }
      },
      error: function (xhr, status, error) {
        console.error('Error:', error);
        $('#connectionStatus').text('Status: Connection Error');
      }
    });
  }

  function goToMode() {
    window.location.href = '/mode'; // Redirect ke halaman mode
  }

  function showResetModal() {
    $('#resetModal').show();
  }

  function closeResetModal() {
    $('#resetModal').hide();
  }

  function resetESP32() {
    $.ajax({
      url: '/reset_esp32/',
      type: 'POST',
      success: function (data) {
        if (data.status === 'success') {
          alert('ESP32 reset successfully');
          closeResetModal();
        } else {
          alert('Failed to reset ESP32');
        }
      },
      error: function (xhr, status, error) {
        console.error('Error:', error);
        alert('Error resetting ESP32');
      }
    });
  }

  // Untuk menutup modal saat klik di luar modal
  $(window).on('click', function (event) {
    if (event.target.id == 'resetModal') {
      closeResetModal();
    }
  });

