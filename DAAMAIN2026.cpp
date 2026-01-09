#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <iomanip>
#include <cctype>
#include <limits>
#include <algorithm>
#include <map>
using namespace std;

// ---------------- STRUCTURES ----------------
struct Item {
    int id;
    string name;
    int quantity;
    string location;
};

struct UndoAction {
    Item item;
};

// ---------------- GLOBAL INDEX ----------------
// Maintain a sorted index for fast lookups
map<string, int> nameToId; // lowercase name -> item id

// ---------------- INPUT BUFFER FIX ----------------
void clearInput() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ---------------- UTILITY ----------------
string toLowerCase(const string &s) {
    string r;
    for (char c : s) r += tolower(c);
    return r;
}

void displayHeader() {
    cout << left << setw(5) << "ID"
         << setw(20) << "Name"
         << setw(10) << "Qty"
         << setw(15) << "Location" << endl;
    cout << string(50, '-') << endl;
}

void displayItem(const Item &i) {
    cout << left << setw(5) << i.id
         << setw(20) << i.name
         << setw(10) << i.quantity
         << setw(15) << i.location << endl;
}

// ---------------- REBUILD INDEX ----------------
void rebuildIndex(const vector<Item> &inv) {
    nameToId.clear();
    for (const auto &i : inv) {
        nameToId[toLowerCase(i.name)] = i.id;
    }
}

// ---------------- FIND ITEM BY ID ----------------
int findItemIndexById(const vector<Item> &inv, int id) {
    for (int i = 0; i < inv.size(); i++) {
        if (inv[i].id == id) return i;
    }
    return -1;
}

// ---------------- FIND ITEM BY NAME (USING INDEX) ----------------
int findItemIndexByName(const vector<Item> &inv, const string &name) {
    string key = toLowerCase(name);
    auto it = nameToId.find(key);
    if (it == nameToId.end()) return -1;

    int id = it->second;
    return findItemIndexById(inv, id);
}

// ---------------- MERGE SORT ----------------
void merge(vector<Item> &a, int l, int m, int r) {
    vector<Item> L(a.begin() + l, a.begin() + m + 1);
    vector<Item> R(a.begin() + m + 1, a.begin() + r + 1);

    int i = 0, j = 0, k = l;
    while (i < L.size() && j < R.size()) {
        if (toLowerCase(L[i].name) <= toLowerCase(R[j].name))
            a[k++] = L[i++];
        else
            a[k++] = R[j++];
    }
    while (i < L.size()) a[k++] = L[i++];
    while (j < R.size()) a[k++] = R[j++];
}

void mergeSort(vector<Item> &a, int l, int r) {
    if (l < r) {
        int m = (l + r) / 2;
        mergeSort(a, l, m);
        mergeSort(a, m + 1, r);
        merge(a, l, m, r);
    }
}

// ---------------- ADD PRODUCT ----------------
void addItem(vector<Item> &inv) {
    Item x;

    cout << "Enter Item ID (unique): ";
    cin >> x.id;
    clearInput();

    for (auto &i : inv)
        if (i.id == x.id) {
            cout << "ID already exists!\n";
            return;
        }

    cout << "Enter Item Name: ";
    getline(cin, x.name);

    // Check if name already exists
    if (nameToId.find(toLowerCase(x.name)) != nameToId.end()) {
        cout << "Product name already exists!\n";
        return;
    }

    cout << "Enter Quantity: ";
    cin >> x.quantity;
    clearInput();

    cout << "Enter Location: ";
    getline(cin, x.location);

    inv.push_back(x);
    nameToId[toLowerCase(x.name)] = x.id; // Update index
    cout << "Product added successfully!\n";
}

// ---------------- VIEW ALL ----------------
void viewAll(const vector<Item> &inv) {
    if (inv.empty()) {
        cout << "Inventory is empty.\n";
        return;
    }
    displayHeader();
    for (auto &i : inv) displayItem(i);
}

// ---------------- SORT ----------------
void sortItems(vector<Item> &inv) {
    if (inv.empty()) {
        cout << "Inventory is empty.\n";
        return;
    }
    mergeSort(inv, 0, inv.size() - 1);
    cout << "Products sorted successfully.\n";
}

// ---------------- SEARCH (OPTIMIZED) ----------------
void searchItem(const vector<Item> &inv) {
    if (inv.empty()) {
        cout << "Inventory is empty.\n";
        return;
    }

    string name;
    cout << "Enter Product Name to search: ";
    getline(cin, name);

    int idx = findItemIndexByName(inv, name);
    if (idx != -1) {
        displayHeader();
        displayItem(inv[idx]);
    } else {
        cout << "Product not found.\n";
    }
}

// ---------------- REMOVE PRODUCT (OPTIMIZED) ----------------
void removeItemFull(vector<Item> &inv, stack<UndoAction> &undoStack) {
    if (inv.empty()) {
        cout << "Inventory is empty.\n";
        return;
    }

    string name;
    cout << "Enter Product Name to remove completely: ";
    getline(cin, name);

    int idx = findItemIndexByName(inv, name);
    if (idx == -1) {
        cout << "Product not found.\n";
        return;
    }

    undoStack.push({inv[idx]});
    nameToId.erase(toLowerCase(inv[idx].name)); // Update index
    inv.erase(inv.begin() + idx);
    cout << "Product removed successfully.\n";
}

// ---------------- UPDATE QUANTITY (OPTIMIZED) ----------------
void updateQuantity(vector<Item> &inv) {
    if (inv.empty()) {
        cout << "Inventory is empty.\n";
        return;
    }

    string name;
    cout << "Enter Product Name to update: ";
    getline(cin, name);

    int idx = findItemIndexByName(inv, name);
    if (idx == -1) {
        cout << "Product not found.\n";
        return;
    }

    cout << "Current quantity: " << inv[idx].quantity << endl;
    cout << "Enter new quantity: ";
    cin >> inv[idx].quantity;
    clearInput();
    cout << "Quantity updated successfully.\n";
}

// ---------------- LOW STOCK ----------------
void lowStock(const vector<Item> &inv) {
    if (inv.empty()) {
        cout << "Inventory is empty.\n";
        return;
    }

    auto cmp = [](Item a, Item b) { return a.quantity > b.quantity; };
    priority_queue<Item, vector<Item>, decltype(cmp)> pq(cmp);

    for (auto &i : inv) pq.push(i);

    displayHeader();
    while (!pq.empty()) {
        displayItem(pq.top());
        pq.pop();
    }
}

// ---------------- UNDO ----------------
void undoRemove(vector<Item> &inv, stack<UndoAction> &st) {
    if (st.empty()) {
        cout << "Nothing to undo.\n";
        return;
    }

    Item restoredItem = st.top().item;

    // Check if ID or name already exists
    for (const auto &i : inv) {
        if (i.id == restoredItem.id) {
            cout << "Cannot undo: ID already exists in current inventory.\n";
            st.pop();
            return;
        }
    }

    if (nameToId.find(toLowerCase(restoredItem.name)) != nameToId.end()) {
        cout << "Cannot undo: Product name already exists in current inventory.\n";
        st.pop();
        return;
    }

    inv.push_back(restoredItem);
    nameToId[toLowerCase(restoredItem.name)] = restoredItem.id; // Update index
    st.pop();
    cout << "Undo successful.\n";
}

// ---------------- MAIN ----------------
int main() {
    vector<Item> inventory;
    stack<UndoAction> undoStack;
    int choice;

    // Build initial index
    rebuildIndex(inventory);

    do {
        cout << "\n========== INVENTORY MANAGEMENT SYSTEM ==========\n";
        cout << "1. Add Product\n";
        cout << "2. Remove Product (Full)\n";
        cout << "3. View All Products\n";
        cout << "4. Sort Products by Name\n";
        cout << "5. Search Product by Name\n";
        cout << "6. Show Low Stock Products\n";
        cout << "7. Update Product Quantity\n";
        cout << "8. Undo Last Removed Product\n";
        cout << "9. Exit\n";
        cout << "=================================================\n";
        cout << "Enter your choice: ";

        cin >> choice;
        clearInput();

        switch (choice) {
            case 1: addItem(inventory); break;
            case 2: removeItemFull(inventory, undoStack); break;
            case 3: viewAll(inventory); break;
            case 4: sortItems(inventory); break;
            case 5: searchItem(inventory); break;
            case 6: lowStock(inventory); break;
            case 7: updateQuantity(inventory); break;
            case 8: undoRemove(inventory, undoStack); break;
            case 9: cout << "Exiting... Thank you!\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 9);

    return 0;
}
