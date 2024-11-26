const express = require('express');
const router = express.Router();
const authController = require('../controllers/authController');

router.post('/auth', authController.authenticateUser);

module.exports = router;