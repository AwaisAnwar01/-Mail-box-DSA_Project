#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
struct Node {
    int id;
    string message;
    Node* left;
    Node* right;
};
void createAccount() {
string userId;
string password;


cout << "Enter a new user id: ";
cin >> userId;
cout << "Enter a new password: ";
cin >> password;

ofstream loginFile("login_info.csv", ios::app);
loginFile << userId << "," << password <<endl;
loginFile.close();

cout << "Account created successfully!" << endl;
}

int login() {
string enteredUserId;
string enteredPassword;


cout << "Enter your user id: ";
cin >> enteredUserId;
cout << "Enter your password: ";
cin >> enteredPassword;

ifstream loginFile("login_info.csv");
string line;
while (getline(loginFile, line)) {
    stringstream ss(line);
    string userId;
    string password;
    getline(ss, userId, ',');
    getline(ss, password, ',');
    if (enteredUserId == userId && enteredPassword == password) {
       cout << "Login successful!" << endl;
        return 0;
    }
}
cout << "Invalid user id or password. Please try again." << endl;
return -1;
}


Node* newNode(int id, string message) {
    Node* node = new Node();
    node->id = id;
    node->message = message;
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node* insert(Node* root, int id, string message) {
    if (root == NULL)
        return newNode(id, message);
    if (id < root->id)
        root->left = insert(root->left, id, message);
    else if (id > root->id)
        root->right = insert(root->right, id, message);
    return root;
}

void readData(string fileName, Node*& root) {
    ifstream file(fileName);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string message;
        int id;
        getline(ss, message,',');
        ss >> id;
        cout << "Read: " << message << "," << id << endl;
        root = insert(root, id, message);
    }
}
void inorderTraversal(Node* root) {
    if (root != NULL) {
        inorderTraversal(root->left);
        cout << root->id << "," << root->message << endl;
        inorderTraversal(root->right);
    }
}
void compose() {
    string message;
    string idString;
    int id;
    string existingData;

    cout << "Enter the message: ";
    cin.ignore();
    getline(cin, message);

    cout << "Enter the id: ";
    getline(cin, idString);
    bool isValid = true;
    for (int i = 0; i < idString.length(); i++) {
        if (!isdigit(idString[i])) {
            isValid = false;
            break;
        }
    }

    if (isValid) {
        id = stoi(idString);
        // Open the file in reading mode and read the existing data
        ifstream file("sent_box.csv");
        existingData = string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
        file.close();
        // Add the new message and id at the top of the existing data
        existingData = message + "," + idString + "\n" + existingData;
        // Open the file in truncate mode and write the entire data to the file
        ofstream fileOut("sent_box.csv", std::ios_base::trunc);
        fileOut << existingData;
        fileOut.close();
    } else {
        cout << "Invalid id entered";
    }
}


// function that searches the node
Node* search(Node* root, int id, string word, int choice) {
    if (root == NULL) {
        return NULL;
    }
    if(choice == 1)
    {
        if(root->id == id)
        {
            return root;
        }
        if (id < root->id)
            return search(root->left, id, word, choice);
        else
            return search(root->right, id, word, choice);
    }
    else
    {
        if (root->message.find(word) != string::npos)
        {
            return root;
        }
        Node* left_search = search(root->left, id, word, choice);
        if(left_search != NULL)
        {
            return left_search;
        }
        return search(root->right, id, word, choice);
    }
}



void printNode(Node* node) {
    if (node != NULL) {
        cout << "id: " << node->id << ", message: " << node->message << endl;
    } else {
        cout << "Node not found" << endl;
    }
}
//  function that deletes nodes from the tree 
void deleteNode(Node*& root, int id, string word, int choice) {
    Node* nodeToDelete = NULL;
    if (choice == 1) {
        nodeToDelete = search(root, id, word, choice);
    } else if (choice == 2) {
        nodeToDelete = search(root, id, word, choice);
    }

    if (nodeToDelete == NULL) {
        cout << "Node doesn't exist" << endl;
        return;
    }

    // Find the node with minimum value in the right subtree
    Node* minNode = nodeToDelete->right;
    while (minNode->left != NULL) {
        minNode = minNode->left;
    }

    // Replace the nodeToDelete's id and message with the minimum node
    nodeToDelete->id = minNode->id;
    nodeToDelete->message = minNode->message;

    // Delete the minimum node
    if (minNode->right != NULL) {
        deleteNode(minNode, minNode->id, minNode->message, 1);
    } else {
        if (minNode->left != NULL) {
            minNode->left = NULL;
        }
        if (minNode->right != NULL) {
            minNode->right = NULL;
        }
        delete minNode;
    }
    //Search for the node again
    Node* deletedNode = search(root, id, word, choice);
    if(deletedNode == NULL){
        cout << "Node with id: " << id << "and message: " << word << " is deleted"<< endl;
    }
}
//  function that deletes messages from the file
void deleteFromFile(string fileName, int id, string word, int choice) {
    ifstream file(fileName);
    ofstream tempFile("temp.csv");
    string line;
    string message;
    int lineId;
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, message, ',');
        ss >> lineId;
        if ((choice == 1 && id != lineId) || (choice == 2 && word != message)) {
            tempFile << message << "," << lineId << endl;
        }
    }
    file.close();
    tempFile.close();
    remove(fileName.c_str());
    rename("temp.csv", fileName.c_str());
}
// function that archives or unarchives the messages
void archive(Node*& root) {
    int id, choice;
    string fileName = "inbox.csv";
    cout << "Enter 1 to archive a message, 2 to unarchive a message: ";
    cin >> choice;
    if (choice == 1) {
        cout << "Enter the id of the message to archive: ";
        cin >> id;
        Node* node = search(root, id, "", 1);
        if (node != NULL) {
            ofstream archiveFile("archived_messages.csv", ios::app);
            archiveFile << node->message << "," << node->id << endl;
            archiveFile.close();
            deleteNode(root,  id, "", 1);
            deleteFromFile(fileName, id, "", 1);
        } else {
            cout << "Message not found" << endl;
        }
    } else if (choice == 2) {
        cout << "Enter the id of the message to unarchive: ";
        cin >> id;
        ifstream archiveFile("archived_messages.csv");
        ofstream tempFile("temp.csv");
        string line;
        string message;
        int lineId;
        bool found = false;
        while (getline(archiveFile, line)) {
            stringstream ss(line);
            getline(ss, message, ',');
            ss >> lineId;
            if (id != lineId) {
                tempFile << message << "," << lineId << endl;
            } else {
                found = true;
                root = insert(root, lineId, message);
                ofstream inboxFile(fileName, ios::app);
                inboxFile << message << "," << lineId << endl;
                inboxFile.close();
            }
        }
        if (!found) {
            cout << "Message not found" << endl;
        }
        archiveFile.close();
        tempFile.close();
        remove("archived_messages.csv");
        rename("temp.csv", "archived_messages.csv");
    } else {
        cout << "Invalid choice" << endl;
    }
}
// main function
int main() {
    Node* root = NULL;

    // Read data from inbox.csv and insert into binary tree
    readData("inbox.csv", root);
    // Read data from sent_box.csv and insert into binary tree
    readData("sent_box.csv", root);

    int choice;
    do {
        cout << "Menu:" << endl;
        cout << "1. If you want to   signup" << endl;
        cout << "2. If you want to  login" << endl;
        cout << "0. If you want to  exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        int id;
        string word;
        switch (choice) {
            case 1: {
                 createAccount();
            }break;
            case 2: {
                if(login()==-1)
                {break;}
                else{
                int choice1;
    do {
        cout << "Menu:" << endl;
        cout << "1. If you want to  Search message by id" << endl;
        cout << "2. If you want to  Search message by word" << endl;
        cout << "3. If you want to Delete message  by id or word" << endl;
        cout << "4. If you want to Compose Message" << endl;
        cout << "5. If you want to archive a message" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice1;
        int id;
        string word;
        switch (choice1) {
            
            case 1: {
                cout << "Enter id to search: ";
                cin >> id;
                Node* result = search(root, id, word, 1);
                if (result != NULL) {
                    cout << "ID: " << result->id << endl;
                    cout << "Message: " << result->message << endl;
                } else {
                    cout << "Node doesn't exist" << endl;
                }
                break;
            }
            case 2: {
                cout << "Enter word to search: ";
                cin >> word;
                Node* result = search(root, id, word, choice);
                if (result != NULL) {
                    cout << "ID: " << result->id << endl;
                    cout << "Message: " << result->message << endl;
                } else {
                    cout << "Node doesn't exist" << endl;
                }
                break;
            }
            case 3: {
                cout << "Enter 1 to delete by id, 2 to delete by word: ";
                cin >> choice;
                if (choice == 1) {
                    cout << "Enter id to delete: ";
                    cin >> id;
                }
                else {
                    cout << "Enter word to delete: ";
                    cin >> word;
                }
                Node* result = search(root, id, word, choice);
                if (result != NULL) {
                  //  root = deleteNode(root, id, word, choice);
                    cout << "Node deleted" << endl;
                    deleteFromFile("inbox.csv", id, word, choice);
                    deleteFromFile("sent_box.csv", id, word, choice);
                }
                else {
                    cout << "Node doesn't exist" << endl;
                }
                break;
            }
            case 4: {
              
          compose();
          break;
            }

            case 5: {
               
            archive(root);
                
            }break;
            case 6: {

                break;
            }break;
            default: {
                cout << "Invalid choice" << endl;
                
            }break;
        }
    } while (choice1 != 6);}}
    break;

       case 0:
        {break;}break;     
        }
        
    } while (choice != 0);
      return 0;
}

            