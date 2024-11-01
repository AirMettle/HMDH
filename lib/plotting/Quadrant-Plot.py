import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import argparse


# Function to process the CSV data
def process_csv_data(csv_path):
    # Read the CSV data into a DataFrame
    df = pd.read_csv(csv_path)

    # Group by Sign, Exponent, and X-offset, then aggregate the Y-offset counts
    df = df.groupby(['Sign', 'Exponent', 'X-offset'], as_index=False)['Y-offset'].sum()

    # Determine the maximum Y-value for scaling
    max_y_value = df['Y-offset'].max()

    # Normalize the Y-values for linear scaling
    df['Y-linear'] = df['Y-offset'] / max_y_value

    # Replace zeros with NaN for logarithmic scaling to avoid log(0)
    df['Y-offset-log'] = df['Y-offset'].replace(0, np.nan)

    # Normalize the Y-values for logarithmic scaling
    df['Y-logarithmic'] = np.log(df['Y-offset-log']) / np.log(max_y_value)

    # Drop the temporary column used for handling zeros
    df.drop('Y-offset-log', axis=1, inplace=True)

    return df

def remap_x_offset_negative(x):
    sequence_negative = [
        3, 7, 11, 15, 19, 23, 27, 31, 2, 6, 10, 14, 18, 22, 26, 30,
        1, 5, 9, 13, 17, 21, 25, 29, 0, 4, 8, 12, 16, 20, 24, 28,
        34, 35, 32, 60, 61, 62, 63, 56, 57, 58, 59, 52, 53, 54, 55,
        48, 49, 50, 51, 44, 45, 46, 47, 33, 40, 41, 42, 43, 36, 37, 38, 39
    ]

    # Create a mapping from the original value to the new value
    mapping_negative = {original: new for new, original in enumerate(sequence_negative)}

    # Return the remapped value or the original if it's not specified in the mapping
    return mapping_negative.get(x, x)

def remap_x_offset_positive(x):

    sequence = [
        60, 61, 62, 63, 56, 57, 58, 59,
        52, 53, 54, 55, 48, 49, 50, 51,
        44, 45, 46, 47, 40, 41, 42, 43,
        36, 37, 38, 39, 32, 33, 34, 35,
        0, 4, 8, 12, 16, 20, 24, 28,
        1, 5, 9, 13, 17, 21, 25, 29,
        2, 6, 10, 14, 18, 22, 26, 30,
        3, 7, 11, 15, 19, 23, 27, 31
    ]

    # Create a mapping from the original value to the new value
    mapping = {original: new for new, original in enumerate(sequence)}

    # Return the remapped value or the original if it's not specified in the mapping
    return mapping.get(x, x)

def plot_data_separate_exponents(df, scale_type='Y-linear'):
    plt.rcParams.update({'font.size': 16})  # Set a larger font size for all plot elements

    # Split the DataFrame based on exponent sign
    df_positive_exp = df[df['Exponent'] == 0].copy()
    df_negative_exp = df[df['Exponent'] == 1].copy()

    # Ensure there's at least one row to avoid NaN/Inf limits
    if df_positive_exp.empty and df_negative_exp.empty:
        print("Both positive and negative exponent dataframes are empty. No plots will be generated.")
        return

    for df_subset, exponent_type in zip([df_positive_exp, df_negative_exp], ['Positive', 'Negative']):
        if df_subset.empty:
            print(f"No data for {exponent_type} exponents. Skipping plot.")
            continue

        # Apply remapping based on exponent type
        if exponent_type == 'Positive':
            df_subset['X-offset'] = df_subset['X-offset'].apply(remap_x_offset_positive)
        elif exponent_type == 'Negative':
            df_subset['X-offset'] = df_subset['X-offset'].apply(remap_x_offset_negative)

        fig, ax = plt.subplots(figsize=(24, 12))

        max_y_value = df[scale_type].max()
        min_positive_value = df[df[scale_type] > 0][scale_type].min()
        ax.set_xlim(-63, 63)
        ax.set_ylim(0, max(max_y_value, 1))

        if scale_type == 'Y-logarithmic':
            y_ticks = [10 ** i for i in range(int(np.ceil(np.log10(max_y_value))) + 1)]
            y_tick_labels = [f"{10 ** i:.1e}" for i in range(int(np.ceil(np.log10(max_y_value))) + 1)]
        else:
            y_ticks = [0, 0.25 * max_y_value, 0.5 * max_y_value, 0.75 * max_y_value, max_y_value]
            y_tick_labels = ['0%', '25%', '50%', '75%', '100%']

        x_ticks = list(range(-60, 64, 10))
        x_tick_labels = [str(abs(x)) for x in x_ticks]

        for sign in [0, 1]:
            quadrant_df = df_subset[df_subset['Sign'] == sign]
            x_values = quadrant_df['X-offset'] * (-1 if sign == 1 else 1)
            y_values = quadrant_df[scale_type]
            ax.bar(x_values, y_values, width=0.50, label=f'Sign {"-" if sign == 1 else "+"}')

        title = f'{exponent_type} Exponents - 100% of Max Value {"(Linear)" if scale_type == "Y-linear" else "(Logarithmic)"}'
        ax.set_title(title, fontsize=18)
        ax.set_xlabel(' ', fontsize=12)
        ax.set_ylabel(' ', fontsize=12)
        ax.set_xticks(x_ticks)
        ax.set_xticklabels(x_tick_labels, fontsize=14)
        ax.set_yticks(y_ticks)
        ax.set_yticklabels(y_tick_labels, fontsize=14)
        ax.spines['bottom'].set_position('zero')
        ax.spines['left'].set_position('zero')
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)
        ax.legend(fontsize=14)

        output_filename = f'../{exponent_type.lower()}-exponents-plot_{"linear" if scale_type == "Y-linear" else "logarithmic"}.png'
        plt.savefig(output_filename)
        plt.close()


# Main function to parse command line arguments and execute the code
def main():
    # Set up argument parsing
    parser = argparse.ArgumentParser(description="Process a CSV file and plot data.")
    parser.add_argument("csv_path", type=str, help="Path to the CSV file")
    args = parser.parse_args()

    # Process the CSV and plot the data
    processed_df = process_csv_data(args.csv_path)
    plot_data_separate_exponents(processed_df, 'Y-linear')
    plot_data_separate_exponents(processed_df, 'Y-logarithmic')

# Call the main function if the script is executed
if __name__ == "__main__":
    main()