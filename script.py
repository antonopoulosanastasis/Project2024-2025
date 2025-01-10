import os
import matplotlib.pyplot as plt

def read_file(file_path):
    """Read the file and extract the relevant data."""
    data = []
    try:
        with open(file_path, 'r') as file:
            print(f"Reading file: {file_path}")  # Debugging: print the file being read
            for line in file:
                line = line.strip()
                if line.startswith('File:'):  # Skip header line
                    continue
                # Assuming each line contains tab-separated values, adjust if needed
                parts = line.split()  # Split by any whitespace (works for space, tab)
                if len(parts) >= 6:  # Expecting at least 6 columns
                    instance, before, after, steiner, convergence_value, energy = parts
                    data.append({
                        'instance': instance.strip(),  # Strip any extra spaces
                        'energy': float(energy)
                    })
    except Exception as e:
        print(f"Error reading file {file_path}: {e}")
    return data

def shorten_instance_name(instance_name):
    """Shorten the instance name for better readability in the plot and remove 'challenge_instances'."""
    # Remove 'challenge_instances' from the instance name
    instance_name = instance_name.replace("challenge_instances/", "")
    # Split by the '_' and take only the first two parts or truncate to the first 10 characters.
    parts = instance_name.split('_')
    if len(parts) > 1:
        return parts[0] + '_' + parts[1]  # Take first two parts like 'point-set_10'
    else:
        return instance_name[:10]  # Truncate to first 10 characters if no '_'

def plot_case_performance(case_name, local_data, sa_data, ant_data):
    """Generate and display a bar plot comparing the performance of the methods for the given case."""
    instances = [shorten_instance_name(entry['instance']) for entry in local_data]  # Shorten instance names
    local_energies = [entry['energy'] for entry in local_data]
    sa_energies = [entry['energy'] for entry in sa_data]
    ant_energies = [entry['energy'] for entry in ant_data]

    # Create a bar chart
    x = range(len(instances))  # X positions for the bars
    width = 0.25  # Width of each bar

    plt.figure(figsize=(12, 6))  # Increase figure size for better readability

    # Create bars for each method
    plt.bar([i - width for i in x], local_energies, width=width, label='Local', color='b')
    plt.bar([i for i in x], sa_energies, width=width, label='SA', color='g')
    plt.bar([i + width for i in x], ant_energies, width=width, label='Ant', color='r')

    # Labels and title
    plt.xlabel('Instance')
    plt.ylabel('Energy')
    plt.title(f'Performance Comparison for {case_name}')
    plt.xticks(x, instances, rotation=90)
    plt.legend()

    # Display the plot
    plt.tight_layout()
    plt.show()

def compare_methods_for_all_cases(case_names, methods_dir):
    """Compare methods for all cases and plot the performance."""
    for case_name in case_names:
        print(f"\nEvaluating {case_name}...")

        # Read data for each method
        local_data = read_file(os.path.join(methods_dir, f'local_{case_name}.txt'))
        sa_data = read_file(os.path.join(methods_dir, f'sa_{case_name}.txt'))
        ant_data = read_file(os.path.join(methods_dir, f'ant_{case_name}.txt'))

        # Plot performance comparison for each case
        plot_case_performance(case_name, local_data, sa_data, ant_data)

if __name__ == "__main__":
    # Define the directory where the files are located
    methods_dir = 'results'  # Replace with the correct path to your files

    # Define the case names (for example: 'case_A', 'case_B', etc.)
    case_names = ['case_A', 'case_B', 'case_C', 'case_D', 'case_E']

    # Run the comparison for all cases
    compare_methods_for_all_cases(case_names, methods_dir)
