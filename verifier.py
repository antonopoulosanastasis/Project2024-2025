import os
import json
from cgshop2025_pyutils.data_schemas.instance import Cgshop2025Instance
from cgshop2025_pyutils.data_schemas.solution import Cgshop2025Solution
from cgshop2025_pyutils import verify

# Directories
input_dir = "challenge_instances/"
output_dir = "output/"

# Get list of instance files
input_files = [f for f in os.listdir(input_dir) if f.endswith(".instance.json")]
output_files = {f.split(".")[0]: f for f in os.listdir(output_dir) if f.endswith(".output.json")}

i = 0

for input_file in input_files:
    # Extract instance UID from input file name
    instance_uid = input_file.split(".")[0]

    # Check if the corresponding output file exists
    if instance_uid not in output_files:
        print(f"No corresponding output file for {input_file}")
        continue

    input_filepath = os.path.join(input_dir, input_file)
    output_filepath = os.path.join(output_dir, output_files[instance_uid])

    # Load data
    with open(input_filepath, 'r') as file:
        data_in = json.load(file)

    with open(output_filepath, 'r') as file:
        data_out = json.load(file)

    # Create instance and solution objects
    instance = Cgshop2025Instance(
        instance_uid=data_in["instance_uid"],
        num_points=data_in["num_points"],
        points_x=data_in["points_x"],
        points_y=data_in["points_y"],
        region_boundary=data_in["region_boundary"],
        num_constraints=data_in["num_constraints"],
        additional_constraints=data_in["additional_constraints"],
    )

    solution = Cgshop2025Solution(
        content_type="CG_SHOP_2025_Solution",
        instance_uid=data_out["instance_uid"],
        steiner_points_x=data_out["steiner_points_x"],
        steiner_points_y=data_out["steiner_points_y"],
        edges=data_out["edges"],
    )

    # Verify the solution
    result = verify(instance, solution, strict=True)

    print(f"\nResults for {input_file}:")
    if result.num_obtuse_triangles != -1:
        print(f"  No. obtuse triangles: {result.num_obtuse_triangles}")
        print(f"  No. Steiner points: {result.num_steiner_points}")
        i = i + 1
    else:
        print("  Errors:")
        for err in result.errors:
            print(f"  - {err}")

print(f"Total tests passed: {i}")