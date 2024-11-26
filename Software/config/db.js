const mongoose = require('mongoose');

/**
 * Connects to the MongoDB instance using Mongoose
 * @function connectDB
 * @returns {Promise} A promise that resolves when the connection is established
 */
const connectDB = async () => {
  try {
    // Verify Mongoose is not null
    if (mongoose === null) {
      throw new Error('mongoose is null');
    }

    // Verify that mongoose.connect is a valid function
    if (typeof mongoose.connect !== 'function') {
      throw new Error('mongoose.connect is not a function');
    }

    // Connect to MongoDB
    const connection = await mongoose.connect('mongodb://localhost:27017/esp32DB', {
      useNewUrlParser: true,
      useUnifiedTopology: true,
    });

    // Verify the connection is not null
    if (connection === null) {
      throw new Error('mongoose connection is null');
    }

    // Verify the connection is ready
    if (connection.readyState !== 1) {
      throw new Error('mongoose connection is not ready');
    }

    console.log('MongoDB connected');
  } catch (err) {
    console.error('Error connecting to MongoDB:', err);
    if (err.name === 'MongoError' && err.code === 'ECONNREFUSED') {
      console.error('Make sure MongoDB is running on port 27017');
    }
    process.exit(1);
  }
};

module.exports = connectDB;