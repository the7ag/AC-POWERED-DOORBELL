const crypto = require('crypto');

/**
 * Generates a random API key as a hexadecimal string
 * @function generateApiKey
 * @returns {String} A random API key as a hexadecimal string
 */
exports.generateApiKey = () => {
  // Verify that crypto is not null
  if (crypto === null) {
    throw new Error('crypto is null');
  }

  // Verify that crypto.randomBytes is a valid function
  if (typeof crypto.randomBytes !== 'function') {
    throw new Error('crypto.randomBytes is not a function');
  }

  try {
    // Generate a random API key as a hexadecimal string
    const apiKey = crypto.randomBytes(16).toString('hex');

    // Verify the API key is not null
    if (apiKey === null) {
      throw new Error('API key is null');
    }

    return apiKey;
  } catch (err) {
    console.error('Error generating API key:', err);
    throw err;
  }
};
