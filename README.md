# ESP32 AC Powered with Node.js API to data log

This project uses an **ESP32** microcontroller to send signal data (when a digital pin goes HIGH) to a **Node.js** backend API. The backend uses **MongoDB** to store user data and the signal data. The ESP32 device sends the data to the server only when the signal on a specific pin goes HIGH, with API key-based authentication.

## Table of Contents
- [Project Overview](#project-overview)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Project Setup](#project-setup)
- [Backend API Structure](#backend-api-structure)
- [ESP32 Setup](#esp32-setup)
- [Database Schema](#database-schema)
- [API Endpoints](#api-endpoints)
- [Testing](#testing)
- [Images](#images)

## Project Overview
This project allows you to authenticate a user and send digital signal data from the ESP32 to a backend server using an API key. When the pin state of a digital input (connected to pin 13) goes HIGH, the ESP32 sends a POST request to the backend API with a timestamp and pin state.

The backend server:
1. Validates the API key sent with each request.
2. Stores signal data in the **SignalData** collection in MongoDB.
3. Associates the data with a user based on their **unique ID** and **API key**.

## Hardware Requirements
- **ESP32 Development Board**
- **Jumper wires** for connecting to external pins
- A breadboard (optional)

### Connections:
- **Pin 13**: Connected to a digital signal source (e.g., a button or sensor)

## Software Requirements
- **Arduino IDE** with the ESP32 board package installed
- **Node.js** and **npm** for running the backend server
- **MongoDB** (local or cloud) to store user and signal data
- **Arduino JSON** library for handling JSON data in the ESP32

## Project Setup

### 1. Clone the Repository
Clone the repository to your local machine:
```bash
git clone https://github.com/yourusername/esp32-signal-data-logger.git
```

### 2. Set Up the Backend API

#### Backend Requirements:
- **Node.js** and **npm** installed
- **MongoDB** running locally or using a cloud service (e.g., MongoDB Atlas)

To set up the backend:
1. Navigate to the `backend` directory:
   ```bash
   cd backend
   ```

2. Install the required dependencies:
   ```bash
   npm install
   ```

3. Start the backend server:
   ```bash
   npm start
   ```

The backend API will be accessible at `http://localhost:5000`.

### 3. Set Up the ESP32

#### ESP32 Code:
1. Open the Arduino IDE and set the board to **ESP32**.
2. Install the **ArduinoJson** library using the Library Manager.
3. Open the ESP32 code, modify the Wi-Fi credentials and backend URL as needed.
4. Upload the code to the ESP32.

### 4. Configure MongoDB
Ensure that MongoDB is running, and update the database connection settings in the backend code (if necessary).

## Backend API Structure

The backend consists of two main collections in MongoDB:
1. **Users**: Stores the user's `uniqueID` and their `apiKey`.
2. **SignalData**: Stores the signal data, including the `timestamp` and `pinState`, linked to the user by their `uniqueID`.

### Database Models:
#### User Schema:
- **uniqueID**: A unique identifier for each user (e.g., from ESP32).
- **apiKey**: A unique API key for the user.

#### SignalData Schema:
- **userID**: The `uniqueID` of the user who owns the signal data.
- **timestamp**: The timestamp when the pin state went HIGH.
- **pinState**: The state of the pin (HIGH/LOW).

## API Endpoints

### 1. **Authentication (POST /auth)**
- **Description**: Authenticates the user and returns an API key.
- **Request Body**:
  ```json
  {
    "unique_id": "USER_UNIQUE_ID"
  }
  ```
- **Response**:
  ```json
  {
    "api_key": "USER_API_KEY"
  }
  ```

### 2. **Save Signal Data (POST /data)**
- **Description**: Saves the signal data when the pin goes HIGH.
- **Request Headers**:
  - **API-Key**: The user's API key.
- **Request Body**:
  ```json
  {
    "timestamp": "timestamp_value",
    "pin_state": "HIGH"
  }
  ```
- **Response**:
  ```json
  {
    "message": "Signal data saved successfully"
  }
  ```

## Testing

### 1. Testing the API:
You can use tools like **Postman** or **curl** to test the API endpoints.

- **Authenticate User**: Send a `POST` request to `/auth` with the `unique_id`.
- **Submit Signal Data**: Send a `POST` request to `/data` with the API key in the headers and the signal data in the body.

### 2. Testing the ESP32:
- Once the ESP32 is running, it will automatically connect to the Wi-Fi, authenticate with the backend, and send signal data when the digital pin goes HIGH.

## Images

### 1. ESP32 Pin Connection Diagram
![image](https://github.com/user-attachments/assets/ec516dd1-dc11-48cf-9433-1d1f7d02cfdd)
### 2. Entire Board
![image](https://github.com/user-attachments/assets/b9d4cad5-9311-491e-86cf-29ba8f5b5d99)

## License
This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.
