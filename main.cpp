#include <iostream>
#include <stdio.h>
#include <unordered_map>
#include <cmath>
#include <fstream>
#include <vector>
#include <ctime>

using namespace std;

double cosineSimilarity(const unordered_map<int, float>& user1, const unordered_map<int, float>& user2) {
    double dotProduct = 0.0;
    double normalUser1 = 0.0;
    double normalUser2 = 0.0;

    for (const auto& movie : user1) {
        int movieId = movie.first;
        float rating1 = movie.second;

        if (user2.count(movieId)) {
            float rating2 = user2.at(movieId);
            dotProduct += rating1 * rating2;
        }
        normalUser1 += rating1 * rating1;
    }

    for (const auto& movie : user2) {
        float rating2 = movie.second;
        normalUser2 += rating2 * rating2;
    }

    if (normalUser1 == 0 || normalUser2 == 0) {
        return 0.0;
    }

    return dotProduct / (sqrt(normalUser1) * sqrt(normalUser2));
}

int main() {
    time_t start = clock();

    string line;
    bool isTest = false;

    int userId = 0;
    int movieId = 0;
    float rating = 0;

    float predictedRating = 0;

    unordered_map<int, unordered_map<int, float>> user_matrix;
    unordered_map<int, unordered_map<int, float>> movie_matrix;

    fstream fio("data.txt", ios::in);

    if(fio.is_open()){
        while (getline(fio, line)) {
            if (line.empty()) {
                continue;
            }
            
            if (line == "train dataset") {
                isTest = false;
                continue;
            }
            if (line == "test dataset") {
                isTest = true;
                continue;
            }

            if (isTest) {
                sscanf(line.c_str(), "%d %d %f", &userId, &movieId);
                
                // User-based prediction
                auto movie_end = movie_matrix.end();
                auto user_end = user_matrix.end();

                auto movie_it = movie_matrix.find(movieId);
                auto user1_it = user_matrix.find(userId);

                double similarity_total = 0.0;
                double rate_total = 0.0;

                if (movie_it == movie_end || user1_it == user_end){
                    predictedRating = 0.0;
                }else{
                    for (const auto& movie : movie_it->second){
                        auto user2_it = user_matrix.find(movie.first);
                        if(user2_it != user_end){
                            double similarity = cosineSimilarity(user1_it->second, user2_it->second);

                            if (similarity > 0){
                                auto user2_rate = user2_it->second.find(movieId);
                                similarity_total += similarity;
                                rate_total += similarity * user2_rate->second;
                            } else {
                                predictedRating = 0.0;
                            }
                        }else{
                            predictedRating = 0.0;
                        }
                    }
                    if (similarity_total > 0){
                        predictedRating = rate_total / similarity_total;
                    }else {
                        predictedRating = 0.0;
                    }
                }

                printf("%.5f\n", predictedRating);


            }
            else {
                sscanf(line.c_str(), "%d %d %f", &userId, &movieId, &rating);
                user_matrix[userId][movieId] = rating;
                movie_matrix[movieId][userId] = rating;
            }
        }
    }else{
        cout << "Error opening file" << endl;
        return 1;
    }
    
    fio.close();

    
    
    time_t finish = clock();
    cout << "Execution time: " << finish - start << endl;
    
    return 0;
}
