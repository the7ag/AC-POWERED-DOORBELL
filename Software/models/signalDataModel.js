const mongoose = require('mongoose');

const signalDataSchema = new mongoose.Schema({
  userID: { type: String, required: true },
  timestamp: { type: String, required: true },
  pinState: { type: String, required: true },
});

module.exports = mongoose.model('SignalData', signalDataSchema);