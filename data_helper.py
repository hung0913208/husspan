import csv

ENDING_ITEMSET = '-1'
ENDING_SEQUENCE = '-2'

INPUT_SEQUENCES_FILE="../cleaned-bms2-dataset/sequences.csv"
INPUT_UTILITIES_FILE="../cleaned-bms2-dataset/utilities.csv"

OUTPUT_DATA_INFO_FILE = "cleaned-bms2-data/info.csv"
OUTPUT_UTILITIES_FILE = "cleaned-bms2-data/utilities.csv"
OUTPUT_REMAINING_FILE = "cleaned-bms2-data/remaining.csv"
OUTPUT_SEQUENCE_UTILITIES_FILE = "cleaned-bms2-data/sequence_utilities.csv"

if __name__ == "__main__":
    items = set()
    '''
        Compute Utilities

        Input:
            sequence: [1, 2, -1, 2, 3, -2]
            utility:  [2, 4,  0, 6, 3,  0]  

        Expected output: (row-item/column-itemset)             
            1   2   0
            2   4   6
            3   0   3

        Compute Remaining Utilities

        Input:
            sequence: [1, 2, -1, 2, 3, -2]
            utility:  [2, 4,  0, 6, 3,  0]

        Expected output: (row-item/column-itemset)             
            1   13   0
            2    9   3
            3    0   0
    '''
    num_sequences = 0
    utilities_output = []
    remaining_utilities_output = []
    sequence_utilities = []
    utilities = []

    with open(INPUT_UTILITIES_FILE, mode='r') as file:
        csv_reader = csv.reader(file, delimiter='\t')
        for row in csv_reader:
            num_sequences += 1
            sequence_utilities.append(round(sum(map(float, row)), 2))
            utilities.append(row)
    
    with open(INPUT_SEQUENCES_FILE, mode='r') as file:
        csv_reader = csv.reader(file, delimiter='\t')
        for row_idx, row in enumerate(csv_reader):
            '''
                row: [1, 2, -1, 2, 3, -2] -> itemset_indices: [2, 5]
            '''
            itemset_indices = [i for i, v in enumerate(row) if v==ENDING_ITEMSET or v==ENDING_SEQUENCE]

            sequence_items = list(set(row))
            if ENDING_ITEMSET in sequence_items: sequence_items.remove(ENDING_ITEMSET)
            sequence_items.remove(ENDING_SEQUENCE)
            sequence_items = sorted(map(int, sequence_items))

            items = items.union(sequence_items)

            for item in sequence_items:
                '''
                    row: [1, 2, -1, 2, 3, -2], item=2 -> item_indices: [1, 3]
                '''
                item_indices = [i for i, v in enumerate(row) if v==str(item)]

                utilities_output_row, remaining_utilities_output_row = [0] * (len(itemset_indices) + 1), [0] * (len(itemset_indices) + 1)
                '''
                    the first elements in these two arrays are the item itself.
                '''
                utilities_output_row[0], remaining_utilities_output_row[0] = item, item
                
                item_index, itemset_index = 0, 0
                item_indices_length, itemset_indices_length = len(item_indices), len(itemset_indices)
                '''
                    Each itemset only has one instance of an item, if that item's index is smaller
                    than that of an itemset, the item belongs to that itemset, otherwise it belongs 
                    to the subsequent itemsets.
                '''
                while item_index < item_indices_length and itemset_index < itemset_indices_length:
                    if item_indices[item_index] < itemset_indices[itemset_index]:
                        '''
                            This item belongs to this itemset. So we save its utility and remaining utility
                            into the matrices. itemset_index+1 is because the first element of a row is occupied
                            by the item itself.
                        '''
                        utilities_output_row[itemset_index+1] = utilities[row_idx][item_indices[item_index]]
                        remaining_utilities_output_row[itemset_index+1] = round(sum(map(float, utilities[row_idx][item_indices[item_index]+1:])), 2)
                        itemset_index += 1
                        item_index += 1
                    else:
                        itemset_index += 1                      
                utilities_output.append(utilities_output_row)
                remaining_utilities_output.append(remaining_utilities_output_row)
            '''
                Append an empyty line to mark the end of a sequence
            '''
            utilities_output.append([])
            remaining_utilities_output.append([])

    items = list(set(items))
    items = sorted(map(int, items))
    num_items = len(items)


    # print(items == list(range(min(items), max(items)+1)))

    # exit(1)

    with open(OUTPUT_DATA_INFO_FILE, mode='w') as file:
        csv_writer = csv.writer(file, delimiter='\t')
        csv_writer.writerow([num_items])
        csv_writer.writerow(items)
        csv_writer.writerow([num_sequences])

    with open(OUTPUT_UTILITIES_FILE, mode='w') as file:
        csv_writer = csv.writer(file, delimiter='\t')
        for utilities_row in utilities_output:
            csv_writer.writerow(utilities_row)

    with open(OUTPUT_REMAINING_FILE, mode='w') as file:
        csv_writer = csv.writer(file, delimiter='\t')
        for remaining_utilities_row in remaining_utilities_output:
            csv_writer.writerow(remaining_utilities_row)

    with open(OUTPUT_SEQUENCE_UTILITIES_FILE, mode='w') as file:
        csv_writer = csv.writer(file, delimiter='\t')
        csv_writer.writerow(sequence_utilities)
