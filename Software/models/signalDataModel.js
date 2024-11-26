const mongoose = require('mongoose');

const signalDataSchema = new mongoose.Schema({
  userID: { type: mongoose.Schema.Types.ObjectId, ref: 'User', required: true },
  timestamp: { type: Date, required: true },
  pinState: { type: String, required: true, enum: ['HIGH', 'LOW'] },
});

module.exports = mongoose.model('SignalData', signalDataSchema);