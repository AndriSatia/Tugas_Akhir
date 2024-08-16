function goToCounter(mode) {
    // Simpan mode yang dipilih dalam local storage
    localStorage.setItem('selectedMode', mode);
    // Redirect ke halaman push_up_counter.html
    window.location.href = '/push-up';
  }

  function goToConnectBLE() {
    window.history.back();
  }

  // Menampilkan modal saat tombol ditekan
  document.getElementById("instructionButton").onclick = function () {
    document.getElementById("instructionModal").style.display = "block";
  }

  // Menutup modal saat tombol close atau area luar modal ditekan
  document.getElementsByClassName("close")[0].onclick = function () {
    document.getElementById("instructionModal").style.display = "none";
  }

  window.onclick = function (event) {
    if (event.target == document.getElementById("instructionModal")) {
      document.getElementById("instructionModal").style.display = "none";
    }
  }

  document.addEventListener('DOMContentLoaded', function () {
    // Initialize Swiper
    var swiper = new Swiper('.swiper-container', {
      loop: true,
      navigation: {
        nextEl: '.swiper-button-next',
        prevEl: '.swiper-button-prev',
      },
      pagination: {
        el: '.swiper-pagination',
        clickable: true,
      },
    });

    // Modal functionality
    var modal = document.getElementById("instructionModal");
    var btn = document.getElementById("instructionButton");
    var span = document.getElementsByClassName("close")[0];

    btn.onclick = function () {
      modal.style.display = "block";
    }

    span.onclick = function () {
      modal.style.display = "none";
    }

    window.onclick = function (event) {
      if (event.target == modal) {
        modal.style.display = "none";
      }
    }
  });

  function changeModeTitle(mode) {
    document.getElementById('mode-title').textContent = 'Mode ' + mode;
  }