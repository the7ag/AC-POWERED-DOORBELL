const crypto = require('crypto');

exports.generateApiKey = () => {
  return crypto.randomBytes(16).toString('hex');
};