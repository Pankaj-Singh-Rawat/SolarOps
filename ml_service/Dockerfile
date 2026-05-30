# Use an official lightweight Python runtime as a parent image
FROM python:3.11-slim

# Set the working directory inside the container
WORKDIR /app

# Copy the requirements file into the container
COPY requirements.txt .

# Install the required Python packages
RUN pip install --no-cache-dir -r requirements.txt

# Copy the rest of the local script into the container
COPY predict.py .

# Expose port 5000 for internal container network communication
EXPOSE 5000

# Run the Flask app when the container launches
CMD ["python", "predict.py"]