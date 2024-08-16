function checkScrollbar() {
    var body = document.body;
    var hasScrollbar = body.scrollHeight > window.innerHeight;
    body.style.overflow = hasScrollbar ? 'auto' : 'hidden';
}

// Panggil fungsi checkScrollbar saat halaman dimuat
window.addEventListener('load', checkScrollbar);

// Panggil fungsi checkScrollbar saat ukuran jendela berubah
window.addEventListener('resize', checkScrollbar);

function goToConnectBLE() {
    window.location.href = '/';
}

$(document).ready(function () {
    // Function to fetch data from Django views.py using AJAX
    function fetchData() {
        $.ajax({
            type: 'GET',
            url: '/data-csv',  // URL to your Django view for fetching data
            success: function (response) {
                // Clear existing table rows
                $('#data-table tbody').empty();

                // Iterate through received data and append rows to the table
                $.each(response.data, function (index, row) {
                    $('#data-table tbody').append(
                        '<tr>' +
                        '<td>' + row.Nama + '</td>' +
                        '<td>' + row.Mode + '</td>' + // Mode ditambahkan ke dalam tabel
                        '<td>' + row["Gerakan Benar"] + '</td>' +
                        '<td>' + row["Gerakan Salah"] + '</td>' +
                        '<td>' + row.Waktu + '</td>' +
                        '</tr>'
                    );
                });
            },
            error: function (xhr, status, error) {
                console.error('Error fetching data:', error);
            }
        });
    }

    // Call fetchData function when the page loads
    fetchData();

    // Function to download data as Excel
    $('#download-btn').click(function () {
        var wb = XLSX.utils.table_to_book(document.getElementById('data-table'), { sheet: "Sheet1" });
        XLSX.writeFile(wb, 'data_push-up.xlsx');
    });

    // Function to clear history with confirmation
    $('#clear-history-btn').click(function () {
        if (confirm('Apakah Anda yakin ingin menghapus semua riwayat?')) {
            $.ajax({
                type: 'POST',
                url: '/clear-history/',  // URL to your Django view for clearing data
                data: {
                    csrfmiddlewaretoken: '{{ csrf_token }}'
                },
                success: function (response) {
                    if (response.status === 'success') {
                        fetchData();  // Refresh the table data after clearing
                    } else {
                        console.error('Error clearing history:', response.message);
                    }
                },
                error: function (xhr, status, error) {
                    console.error('Error clearing history:', error);
                }
            });
        }
    });

    // Function to go back to the previous page
    $('#back-btn').click(function () {
        window.history.back();
    });
});