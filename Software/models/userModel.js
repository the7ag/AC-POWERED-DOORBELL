const mongoose = require('mongoose');

const userSchema = new mongoose.Schema({
  uniqueID: { type: String, required: true, unique: true, dropDups: true },
  apiKey: { type: String, required: true },
});

module.exports = mongoose.model('User', userSchema);