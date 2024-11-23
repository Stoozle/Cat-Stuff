import matplotlib
matplotlib.use('Agg')
import csv
from flask import Flask, request, render_template, send_from_directory, jsonify
import matplotlib.pyplot as plt
import os

app = Flask(__name__)

# Define the CSV file where the data will be appended
FILE = 'data.csv'

# Check if the CSV file exists, and if not, create it with a header row
def initialize_csv():
    try:
        with open(FILE, mode='a', newline='') as file:
            writer = csv.writer(file)
            # Write the header if the file is empty
            if file.tell() == 0:
                writer.writerow(['weight', 'day'])  # Add your header here
    except Exception as e:
        print(f"Error initializing CSV file: {e}")


def read_csv_data():
    # Lists to store the data
    weights = []
    days = []
    
    try:
        # Open the CSV file and read its contents
        with open(FILE, mode='r') as file:
            reader = csv.DictReader(file)  # Use DictReader to read the rows as dictionaries
            
            for row in reader:
                try:
                    # Append data to the lists (convert strings to float for plotting)
                    weights.append(float(row['weight']))
                    days.append(float(row['day']))
                except ValueError:
                    print(f"Skipping invalid row: {row}")
        return weights, days
    except Exception as e:
        print(f"Error reading CSV file: {e}")
        return [], []


def plot_data(weights, days):
    # Plot weight vs. day
    plt.figure(figsize=(10, 6))

    # Scatter plot: day on x-axis, weight on y-axis
    plt.scatter(days, weights, color='tab:blue', label='Data points', alpha=0.7)

    # Add titles and labels
    plt.title('Weight over Time')
    plt.xlabel('Day')
    plt.ylabel('Weight (LBS)')
    plt.legend()

    # Save the plot as an image
    plot_image_path = 'static/plot.png'
    plt.savefig(plot_image_path)

    # Close the plot to avoid displaying it
    plt.close()

    return plot_image_path  # Return the path to the saved image


# Route to serve the saved plot image
@app.route('/plot_image')
def serve_plot():
    return send_from_directory(os.getcwd(), 'static/plot.png')


# Route to display the webpage with the plot
@app.route('/')
def index():
    # Read the data from the CSV file
    weights, days = read_csv_data()

    if weights and days:
        # Generate the plot if data is available
        plot_image_path = plot_data(weights, days)
    else:
        plot_image_path = None  # No data to display

    # Render the template and pass the plot path (or None if no data)
    return render_template('index.html', plot_image_path=plot_image_path)


# Route to handle the form submission and append the data to CSV
@app.route('/submit', methods=['POST'])
def handle_post():
    try:
        # Read raw data from the POST request
        data = request.data.decode('utf-8')  # Decode to string

        if not data:
            return jsonify({'error': 'No data received'}), 400

        # You can now work with the string data
        print("Received data:", data)

        # Split the data by '&' to get individual key-value pairs
        data_pairs = data.split('&')

        # Create a dictionary from the key-value pairs
        data_dict = {}
        for pair in data_pairs:
            key, value = pair.split('=')
            data_dict[key] = value


	 # Append the data to the CSV file
        with open(FILE, mode='a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([data_dict.get('weight', ''), data_dict.get('day', '')])

        # Read the data from the CSV file to generate the plot
        weights, days = read_csv_data()

        # Plot the data if it's available
        if weights and days:
           plot_image_path =  plot_data(weights, days)

        # Respond with a success message and include the path to the plot image
        return jsonify({'message': 'Data received and appended to CSV file', 'received_data': data_dict, 'plot_image': plot_image_path}), 200

    except Exception as e:
        return jsonify({'error': str(e)}), 500


if __name__ == '__main__':
    # Create the static folder if it doesn't exist
    if not os.path.exists('static'):
        os.makedirs('static')

    # Initialize the CSV file if it's empty or doesn't exist
    initialize_csv()
    
    # Start the Flask app
    app.run(debug=True, host='0.0.0.0', port=8000)
