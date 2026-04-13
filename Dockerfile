# Use Node.js 20 as base
FROM node:20

# Install build tools for C++ and OpenMP
RUN apt-get update && apt-get install -y \
    g++ \
    libomp-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy package files
COPY package*.json ./

# Install Node dependencies
RUN npm install

# Copy project files
COPY . .

# Build the C++ binary for Linux
RUN npm run build

# Expose the dashboard port
EXPOSE 3000

# Start the server
CMD ["npm", "start"]
