const User = require('../models/userModel');
const { generateApiKey } = require('../services/authService');

/**
 * Authenticates a user and returns an API key
 * @function authenticateUser
 * @param {Object} req - Express request object
 * @param {Object} res - Express response object
 * @returns {Promise} A promise that resolves with the API key response
 */
exports.authenticateUser = async (req, res) => {
  try {
    // Get the unique ID from the request body
    const { unique_id } = req.body;
    if (!unique_id) {
      return res.status(400).json({ error: 'Unique ID is required' });
    }

    // Find the user associated with the unique ID
    const user = await User.findOne({ uniqueID: unique_id });

    // If the user is not found, generate a new API key
    if (!user) {
      const apiKey = generateApiKey();

      // Create a new user with the given unique ID and API key
      const newUser = new User({
        uniqueID: unique_id,
        apiKey: apiKey,
      });

      // Save the new user
      await newUser.save();

      // Return the API key in the response
      return res.status(200).json({ api_key: apiKey });
    } else {
      // Return the API key associated with the user in the response
      return res.status(200).json({ api_key: user.apiKey });
    }
  } catch (err) {
    // Log any errors to the console
    console.error(err);

    // Return a 500 error response with an error message
    return res.status(500).json({ error: 'Internal Server Error' });
  }
};


