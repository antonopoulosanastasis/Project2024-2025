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
    """Shorten the instance name to keep only the id-like part."""
    # Remove 'challenge_instances' from the instance name
    instance_name = instance_name.replace("challenge_instances/", "")
    
    # Remove the '.instance.json' extension
    instance_name = instance_name.replace('.instance.json', '')
    
    # Find the id-like part by splitting the string at the underscore
    parts = instance_name.split('_')
    
    # Return the last part (id-like part) starting with '_'
    return parts[-1]  # Keep the id-like part (e.g., 'f999dc7f')

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

        # Calculate total energy for each method
        total_local_energy = sum(entry['energy'] for entry in local_data)
        total_sa_energy = sum(entry['energy'] for entry in sa_data)
        total_ant_energy = sum(entry['energy'] for entry in ant_data)

        # Print total energy for each method
        print(f"Total Energy for {case_name}:")
        print(f"  Local: {total_local_energy:.2f}")
        print(f"  SA: {total_sa_energy:.2f}")
        print(f"  Ant: {total_ant_energy:.2f}")

        # Plot performance comparison for each case
        plot_case_performance(case_name, local_data, sa_data, ant_data)

if __name__ == "__main__":
    # Define the directory where the files are located
    methods_dir = 'results'  # Replace with the correct path to your files

    # Define the case names (for example: 'case_A', 'case_B', etc.)
    case_names = ['case_A', 'case_B', 'case_C', 'case_D', 'case_E']

    # Run the comparison for all cases
    compare_methods_for_all_cases(case_names, methods_dir)
