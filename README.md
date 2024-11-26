# BlogApp

## Overview

BlogApp is a simple blog platform where users can create accounts, post blog entries, add comments, and interact with posts through reactions (likes and dislikes). This application uses MySQL as the database to store user, post, comment, and reaction data.

## Features

1. **User Authentication**
   - Users can register with a username, email, and password.
   - Users can log in to the platform using their credentials.

2. **Post Creation and Management**
   - Users can create posts by providing a title and content.
   - Posts are stored in the database with an association to the author's username.

3. **Reactions**
   - Users can react to posts with "like" or "dislike".
   - Reactions are stored in the database and counted per post.

4. **Comments**
   - Users can add comments to posts.
   - Each comment is linked to the post and the author of the comment.

5. **View Posts**
   - Users can view all posts with their likes, dislikes, and comments.
   - Posts can be filtered by the username of the author to display only posts from that user.

6. **Post Update and Deletion**
   - Authors can edit or delete their posts.
   - Updates and deletions are verified by checking the user's credentials.

## Database Structure

The application uses the following tables in MySQL:

- **Users**: Stores user information (username, email, password).
- **Posts**: Stores blog posts with a title, content, and associated author.
- **Comments**: Stores comments on posts with content and author.
- **Reactions**: Stores reactions (like/dislike) on posts.

## Dependencies

- **MySQL**: MySQL server for database management.
- **C++**: The main programming language used.
- **MySQL C++ Connector**: A library to interact with the MySQL database from C++.

### Tables

   CREATE TABLE Users (
       id INT AUTO_INCREMENT PRIMARY KEY,
       username VARCHAR(255) NOT NULL,
       email VARCHAR(255) NOT NULL,
       password VARCHAR(255) NOT NULL
   );

   CREATE TABLE Posts (
       id INT AUTO_INCREMENT PRIMARY KEY,
       title VARCHAR(255) NOT NULL,
       content TEXT NOT NULL,
       author VARCHAR(255) NOT NULL,
       FOREIGN KEY (author) REFERENCES Users(username)
   );

   CREATE TABLE Comments (
       id INT AUTO_INCREMENT PRIMARY KEY,
       content TEXT NOT NULL,
       author VARCHAR(255) NOT NULL,
       post_id INT NOT NULL,
       FOREIGN KEY (post_id) REFERENCES Posts(id),
       FOREIGN KEY (author) REFERENCES Users(username)
   );

   CREATE TABLE Reactions (
       id INT AUTO_INCREMENT PRIMARY KEY,
       post_id INT NOT NULL,
       username VARCHAR(255) NOT NULL,
       reaction_type VARCHAR(10) NOT NULL,  -- "like" or "dislike"
       FOREIGN KEY (post_id) REFERENCES Posts(id),
       FOREIGN KEY (username) REFERENCES Users(username)
   );
   
## Usage

### Register a User
- The application will prompt you to enter a username, email, and password.
- After registering, you can log in with the same credentials.

### Create a Post
- After logging in, you can create a post by providing a title and content.

### React to Posts
- You can react to posts by providing either a "like" or "dislike".

### Comment on Posts
- You can add comments to any post by providing content.

### View Posts
- You can view all posts, including their like, dislike, and comment counts.

### Edit and Delete Posts
- If you are the author of a post, you can edit or delete your posts.

You can update the database connection parameters in the `User`, `Post`, and `Comment` classes if needed, and ensure the MySQL server is running for the app to interact with the database.
