const express = require('express');
const connectDB = require('./config/db');
const authRoutes = require('./routes/authRoutes');
const dataRoutes = require('./routes/dataRoutes');

const app = express();
const port = 5000;

connectDB();

app.use(express.json());

app.use(authRoutes);
app.use(dataRoutes);

app.listen(port, () => {
  console.log(`Server running on port ${port}`);
});