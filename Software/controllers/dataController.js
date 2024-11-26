const User = require('../models/userModel');
const SignalData = require('../models/signalDataModel');

/**
 * Saves the signal data to the database
 * @function saveSignalData
 * @param {Object} req - Express request object
 * @param {Object} res - Express response object
 * @returns {Promise} A promise that resolves with the API key response
 */
exports.saveSignalData = async (req, res) => {
  const { timestamp, pin_state } = req.body;
  const apiKey = req.headers['api-key'];  // Get the API key from the headers

  if (!apiKey || !timestamp || !pin_state) {
    return res.status(400).json({ error: 'Missing required fields' });
  }

  try {
    // Find the user associated with the API key
    const user = await User.findOne({ apiKey: apiKey });

    if (!user) {
      return res.status(403).json({ error: 'Invalid API key' });
    }

    // Retrieve the unique_id from the user record
    const unique_id = user.uniqueID;

    // Save the signal data
    const signalData = new SignalData({
      userID: unique_id,  // Use the unique_id retrieved from the user
      timestamp: timestamp,
      pinState: pin_state,
    });

    await signalData.save();
    res.status(200).json({ message: 'Signal data saved successfully' });
  } catch (error) {
    console.error(error);
    return res.status(500).json({ error: 'Internal Server Error' });
  }
};
