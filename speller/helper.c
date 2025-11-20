#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/**
 * edit_distance(a,b)
 * 
 * TODO: Implement a better version of min_edit_distance that turns O(mxn) space into O(min(m,n)) space
 * 
 * This function calculates the Levenshtein distance between strings a and b. 
 * Levenshtein distance works on the following principle: 
 * Since there are only three ways (insertion, deletion and substitution) to convert between
 * strings with a length difference of 1, we calculate the cost associated with each operation
 * and return the minimum one.
 * Since this problem can be broken down into smaller subparts, we use a DP table to solve it.
 *
 */
int edit_distance(char *a, char *b){
    int m = strlen(a)+1;
    int n = strlen(b)+1;
    // Create a dp table [m x n] 
    // We add 1 to lengths because we look for all possible prefix conversions
    // and empty string is also a possibility
    int dp[m][n];
    // dp[i][j] gives us how many edits we need to convert everything before the last character
    // And then what we do with the last character
    for (int i = 0; i < m;i++)
        for (int j = 0; j < n;j++){
            if (i==0)
            // Going from an empty string to a word of length j means j insertions
            dp[i][j] = j;
            else if (j==0)
            // Going from a word of length i to an empty string means i deletions
                dp[i][j] = i;
            else{
                // If the last letter is the same, there is no substitution cost else it is 1
                int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
                // If A[0...i-2] = B[0...j-1], we delete A[i-1]
                int deletion = dp[i - 1][j] + 1;
                // If A[0...i-1] = B[0...j-2], we insert B[j-1]
                int insertion = dp[i][j - 1] + 1;
                // If A[0...i-1] = B[0...j-1], we replace A[i-1] with B[j-1] depending on cost
                int substitution = dp[i - 1][j - 1] + cost;
                int k = deletion;
                if (insertion<k)
                    k = insertion;
                if (substitution<k)
                    k = substitution;
                // We use the minimum of the three as dp[i][j]
                dp[i][j] = k;
            }
        }
    return dp[m-1][n-1];
}

char *best_match(char *target, char **dictionary, int dict_size){
    int shortestDistance = __INT_MAX__;
    char *bestWord;
    for (int i = 0; i < dict_size; i++)
    {
        char *word = dictionary[i];
        int d = edit_distance(word, target);
        if (d<shortestDistance){
            shortestDistance=d;
            bestWord = word;
        }
        else if (d==shortestDistance){
            int bestSameFirst = bestWord[0] == target[0];
            int currSameFirst = word[0] == target[0];
            if (!bestSameFirst && currSameFirst)
                bestWord = word;
            else if (bestSameFirst==currSameFirst){
                int lenDiffBest = abs((int)strlen(bestWord)-(int)strlen(target));
                int lenDiffCurr = abs((int)strlen(word) - (int)strlen(target));
                if (lenDiffCurr<lenDiffBest)
                    bestWord = word;
            }
        }
    }
    return bestWord;
}