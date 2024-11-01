const form = document.getElementById('uploadForm');
const statusText = document.getElementById('status');

form.addEventListener('submit', async (event) => {
	event.preventDefault(); // Prevent form submission

	const fileInput = document.getElementById('fileInput');
	const file = fileInput.files[0]; // Get the selected file

	if (!file) {
		statusText.textContent = "Please select a file.";
		return;
	}

	// Create a FormData object to hold the file data
	const formData = new FormData();
	formData.append('file', file);

	try {
		// Use fetch to upload the file to the server
		const response = await fetch('/upload/', {
			method: 'POST',
			body: formData
		});

		if (response.ok) {
			statusText.textContent = "File uploaded successfully! Redirecting...";
			window.location.replace("/config/");
		} else {
			statusText.textContent = "File upload failed!";
		}
	} catch (error) {
		statusText.textContent = "An error occurred: " + error.message;
	}
});
