#include <mysql.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

const char* HOST = "localhost";
const char* USER = "root";
const char* PASS = "password";
const char* DB = "dbname";

template <typename T>
std::string to_string(T value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// User Class
class User {
protected:
    string username;
    string email;
    string password;

public:
    User(string uname, string mail, string pass)
        : username(uname), email(mail), password(pass) {}

    bool login(string uname, string pass, MYSQL* conn) {
        string query = "SELECT * FROM Users WHERE username = '" + uname + "' AND password = '" + pass + "'";
        if (mysql_query(conn, query.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
            return false;
        }

        MYSQL_RES* result = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            username = row[1]; // assuming username is at index 1
            email = row[2];    // email at index 2
            password = row[3]; // password at index 3
            return true;
        }
        return false;
    }

    void registerUser(MYSQL* conn) {
        string uname, mail, pass;
        cout << "Enter username: ";
        cin >> uname;
        cout << "Enter email: ";
        cin >> mail;
        cout << "Enter password: ";
        cin >> pass;

        string query = "INSERT INTO Users (username, email, password) VALUES ('" + uname + "', '" + mail + "', '" + pass + "')";
        if (mysql_query(conn, query.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
        } else {
            cout << "Registration successful! You can now log in." << endl;
        }
    }

    string getUsername() const { return username; }
};

// Comment Class
class Comment {
private:
    string content;
    User* author;

public:
    Comment(string cnt, User* auth) : content(cnt), author(auth) {}

    void saveToDB(MYSQL* conn, int postId) {
        string query = "INSERT INTO Comments (content, author, post_id) VALUES ('" + content + "', '" + author->getUsername() + "', " + to_string(postId) + ")";
        if (mysql_query(conn, query.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
        } else {
            cout << "Comment saved successfully!" << endl;
        }
    }
};

class Post {
private:
    string title;
    string content;
    User* author;
    int postId;

public:
    Post(string ttl, string cnt, User* auth, int id = 0) : title(ttl), content(cnt), author(auth), postId(id) {}

    void saveToDB(MYSQL* conn) {
        string query = "INSERT INTO Posts (title, content, author) VALUES ('" + title + "', '" + content + "', '" + author->getUsername() + "')";
        if (mysql_query(conn, query.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
        } else {
            cout << "Post created successfully!" << endl;
        }
    }
      void addReaction(MYSQL* conn, const string& username, const string& reactionType) {
        string query = "INSERT INTO Reactions (post_id, username, reaction_type) VALUES (" + to_string(postId) + ", '" + username + "', '" + reactionType + "')";
        if (mysql_query(conn, query.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
        } else {
            cout << "Reaction added successfully!" << endl;
        }
    }

  void viewPosts(MYSQL* conn) {
    string query = "SELECT * FROM Posts";
    if (mysql_query(conn, query.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    MYSQL_ROW row;
    cout << "\n---- All Posts ----" << endl;

    while ((row = mysql_fetch_row(result))) {
        int postId = atoi(row[0]);  // Post ID at index 0
        string title = row[1];      // Title at index 1
        string content = row[2];    // Content at index 2
        string author = row[3];     // Author at index 3

        // Get the like count
        string likeQuery = "SELECT COUNT(*) FROM reactions WHERE post_id = " + to_string(postId) + " AND reaction_type = 'like'";
        if (mysql_query(conn, likeQuery.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
            continue;
        }

        MYSQL_RES* likeResult = mysql_store_result(conn);
        MYSQL_ROW likeRow = mysql_fetch_row(likeResult);
        int likeCount = atoi(likeRow[0]); // Like count

        // Get the dislike count
        string dislikeQuery = "SELECT COUNT(*) FROM reactions WHERE post_id = " + to_string(postId) + " AND reaction_type = 'dislike'";
        if (mysql_query(conn, dislikeQuery.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
            continue;
        }

        MYSQL_RES* dislikeResult = mysql_store_result(conn);
        MYSQL_ROW dislikeRow = mysql_fetch_row(dislikeResult);
        int dislikeCount = atoi(dislikeRow[0]); // Dislike count

        // Fetch the count and details of comments for this post
        string commentQuery = "SELECT content, author FROM Comments WHERE post_id = " + to_string(postId);
        if (mysql_query(conn, commentQuery.c_str())) {
            cout << "Error: " << mysql_error(conn) << endl;
            continue;
        }

        MYSQL_RES* commentResult = mysql_store_result(conn);
        MYSQL_ROW commentRow;
        int commentCount = 0;  // Initialize comment count to 0

        // First, calculate the comment count by fetching the result
        while ((commentRow = mysql_fetch_row(commentResult))) {
            commentCount++; // Increment for each comment
        }

        // Reset the cursor to the beginning of the result set to display the comments
        mysql_data_seek(commentResult, 0);

        // Display post details with like, dislike, and comment counts
        cout << "Post ID: " << postId << "\nTitle: " << title
             << "\nContent: " << content << "\nAuthor: " << author
             << "\nLikes: " << likeCount << " | Dislikes: " << dislikeCount
             << " | Comments: " << commentCount << endl;
        cout << "---------------------------" << endl;
        cout << "Comments:\n";

        // Display all comments for this post
        while ((commentRow = mysql_fetch_row(commentResult))) {
            string commentContent = commentRow[0]; // Comment content at index 0
            string commentAuthor = commentRow[1];  // Comment author at index 1
            cout << " *" << commentContent << " -" << commentAuthor << "\n\n"; // Display comment content with author
        }

        // Free the results
        mysql_free_result(likeResult);
        mysql_free_result(dislikeResult);
        mysql_free_result(commentResult);
    }

    mysql_free_result(result); // Free the post result
}

// Updated viewPostsByUser Method to Show Likes, Dislikes, and Comments Count for Posts by User
void viewPostsByUser(MYSQL* conn, const string& username) {
    string query = "SELECT * FROM Posts WHERE author = '" + username + "'";
    if (mysql_query(conn, query.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    MYSQL_ROW row;

    if (mysql_num_rows(result) == 0) {
        cout << "No posts found for user: " << username << endl;
    } else {
        cout << "\n---- Posts by " << username << " ----" << endl;
        while ((row = mysql_fetch_row(result))) {
            int postId = atoi(row[0]);  // Post ID at index 0
            string title = row[1];      // Title at index 1
            string content = row[2];    // Content at index 2
            string author = row[3];     // Author at index 3

            // Get the like count
            string likeQuery = "SELECT COUNT(*) FROM reactions WHERE post_id = " + to_string(postId) + " AND reaction_type = 'like'";
            if (mysql_query(conn, likeQuery.c_str())) {
                cout << "Error: " << mysql_error(conn) << endl;
                continue;
            }

            MYSQL_RES* likeResult = mysql_store_result(conn);
            MYSQL_ROW likeRow = mysql_fetch_row(likeResult);
            int likeCount = atoi(likeRow[0]); // Like count

            // Get the dislike count
            string dislikeQuery = "SELECT COUNT(*) FROM reactions WHERE post_id = " + to_string(postId) + " AND reaction_type = 'dislike'";
            if (mysql_query(conn, dislikeQuery.c_str())) {
                cout << "Error: " << mysql_error(conn) << endl;
                continue;
            }

            MYSQL_RES* dislikeResult = mysql_store_result(conn);
            MYSQL_ROW dislikeRow = mysql_fetch_row(dislikeResult);
            int dislikeCount = atoi(dislikeRow[0]); // Dislike count

            // Fetch the count and details of comments for this post
            string commentQuery = "SELECT content, author FROM Comments WHERE post_id = " + to_string(postId);
            if (mysql_query(conn, commentQuery.c_str())) {
                cout << "Error: " << mysql_error(conn) << endl;
                continue;
            }

            MYSQL_RES* commentResult = mysql_store_result(conn);
            MYSQL_ROW commentRow;
            int commentCount = 0;  // Initialize comment count to 0

            // First, calculate the comment count by fetching the result
            while ((commentRow = mysql_fetch_row(commentResult))) {
                commentCount++; // Increment for each comment
            }

            // Reset the cursor to the beginning of the result set to display the comments
            mysql_data_seek(commentResult, 0);

            // Display post details with like, dislike, and comment counts
            cout << "Post ID: " << postId << "\nTitle: " << title
                 << "\nContent: " << content << "\nAuthor: " << author
                 << "\nLikes: " << likeCount << " | Dislikes: " << dislikeCount
                 << " | Comments: " << commentCount << endl;
            cout << "---------------------------" << endl;
            cout << "Comments:\n";

            // Display all comments for this post
            while ((commentRow = mysql_fetch_row(commentResult))) {
                string commentContent = commentRow[0]; // Comment content at index 0
                string commentAuthor = commentRow[1];  // Comment author at index 1
                cout << " *" << commentContent << " -" << commentAuthor << "\n\n"; // Display comment content with author
            }

            // Free the results
            mysql_free_result(likeResult);
            mysql_free_result(dislikeResult);
            mysql_free_result(commentResult);
        }
    }

    mysql_free_result(result); // Free the post result
}


    // Updated updateToDB to check if logged-in user is the author
   void updateToDB(MYSQL* conn, User* loggedInUser) {
    // Check if the logged-in user is the author of the post
    if (author->getUsername() != loggedInUser->getUsername()) {
        cout << "You can only edit your own posts!" << endl;
        return; // Prevent the update if the user is not the author
    }

    // Construct the update query
    string query = "UPDATE Posts SET title = '" + title + "', content = '" + content + "' WHERE id = " + to_string(postId) + " AND author = '" + author->getUsername() + "'";

    // Execute the query
    if (mysql_query(conn, query.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
    } else {
          // Ensure post was deleted by checking the query result
        if (mysql_affected_rows(conn) > 0) {
            cout << "Post updated successfully!" << endl;
        } else {
            cout << "Post deletion failed or post not found." << endl;
        }

    }
}



    // Updated deleteFromDB to check if logged-in user is the author
    void deleteFromDB(MYSQL* conn, User* loggedInUser) {
    if (author->getUsername() != loggedInUser->getUsername()) {
        cout << "You can only delete your own posts!" << endl;
        return;
    }

    string query = "DELETE FROM Posts WHERE id = " + to_string(postId) + " AND author = '" + author->getUsername() + "'";
    if (mysql_query(conn, query.c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
    } else {
        // Ensure post was deleted by checking the query result
        if (mysql_affected_rows(conn) > 0) {
            cout << "Post deleted successfully!" << endl;
        } else {
            cout << "Post deletion failed or post not found." << endl;
        }
    }
}


    int getPostId() const { return postId; }
};

class BlogApp {
private:
    User* loggedInUser;
    MYSQL* conn;

public:
    BlogApp() : loggedInUser(NULL) {
        conn = mysql_init(0);
        if (!mysql_real_connect(conn, HOST, USER, PASS, DB, 3306, NULL, 0)) {
            cout << "ERROR: " << mysql_error(conn) << endl;
        }
    }

    ~BlogApp() { mysql_close(conn); }

    void registerUser() {
        User newUser("", "", "");
        newUser.registerUser(conn);
    }

    void viewPosts() {
        Post post("", "", NULL);
        post.viewPosts(conn);
    }

    void viewPostsByUser() {
        string username;
        cout << "Enter the username to view their posts: ";
        cin >> username;
        Post post("", "", NULL);
        post.viewPostsByUser(conn, username);
    }

    void loginUser() {
        string uname, pass;
        cout << "Enter username: ";
        cin >> uname;
        cout << "Enter password: ";
        cin >> pass;

        User* newUser = new User(uname, "", pass);
        if (newUser->login(uname, pass, conn)) {
            loggedInUser = newUser;
            cout << "Login successful! Welcome, " << loggedInUser->getUsername() << "!" << endl;
        } else {
            delete newUser;
            cout << "Login failed. Please check your credentials." << endl;
        }
    }

    void createPost() {
        if (loggedInUser) {
            string title, content;
            cout << "Enter post title: ";
            cin.ignore();
            getline(cin, title);
            cout << "Enter post content: ";
            getline(cin, content);

            Post newPost(title, content, loggedInUser);
            newPost.saveToDB(conn);
        } else {
            cout << "You need to log in to create a post." << endl;
        }
    }

    void updatePost() {
    if (loggedInUser) {
        int postId;
        string title, content;
        cout << "Enter Post ID to update: ";
        cin >> postId;
        cout << "Enter new title: ";
        cin.ignore(); // To clear any leftover newline character in the buffer
        getline(cin, title); // Get the new title
        cout << "Enter new content: ";
        getline(cin, content); // Get the new content

        // Create a Post object with the provided details and logged-in user
        Post updatedPost(title, content, loggedInUser, postId);
        
        // Call the updateToDB method to update the post in the database
        updatedPost.updateToDB(conn, loggedInUser);
    } else {
        cout << "You need to log in to update a post." << endl;
    }
}


    void deletePost() {
        if (loggedInUser) {
            int postId;
            cout << "Enter Post ID to delete: ";
            cin >> postId;

            Post post("", "", loggedInUser, postId);
            post.deleteFromDB(conn, loggedInUser);
        } else {
            cout << "You need to log in to delete a post." << endl;
        }
    }

    void addCommentToPost() {
        if (loggedInUser) {
            int postId;
            string content;
            cout << "Enter Post ID to add comment: ";
            cin >> postId;
            cout << "Enter comment content: ";
            cin.ignore();
            getline(cin, content);

            Comment newComment(content, loggedInUser);
            newComment.saveToDB(conn, postId);
        } else {
            cout << "You need to log in to comment on a post." << endl;
        }
    }
     void reactToPost() {
        if (loggedInUser) {
            int postId;
            char reaction;
            cout << "Enter Post ID to react to: ";
            cin >> postId;
            cout << "Enter reaction ([L]ike/[D]islike): ";
            cin >> reaction;

            string reactionType = (reaction == 'L' || reaction == 'l') ? "like" : "dislike";
            Post post("", "", loggedInUser, postId);
            post.addReaction(conn, loggedInUser->getUsername(), reactionType);
        } else {
            cout << "You need to log in to react to a post." << endl;
        }
    }
};

// Main Function
int main() {
    BlogApp app;
    int choice;

    while (true) {
        cout << "\n---- Blog Application ----" << endl;
        cout << "1. Register" << endl;
        cout << "2. Login" << endl;
        cout << "3. View Posts" << endl;
        cout << "4. View Posts by User" << endl;
        cout << "5. Create Post" << endl;
        cout << "6. Update Post" << endl;
        cout << "7. Delete Post" << endl;
        cout << "8. Add Comment to Post" << endl;
        cout << "9. React to Post" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                app.registerUser();
                break;
            case 2:
                app.loginUser();
                break;
            case 3:
                app.viewPosts();
                break;
            case 4:
                app.viewPostsByUser();
                break;
            case 5:
                app.createPost();
                break;
            case 6:
                app.updatePost();
                break;
            case 7:
                app.deletePost();
                break;
            case 8:
                app.addCommentToPost();
                break;
            case 9:
            	app.reactToPost();
                break;
            case 0:
                cout << "Exiting..." << endl;
                return 0;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
} 



