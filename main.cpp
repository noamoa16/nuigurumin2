// assertを無効にする
#define NDEBUG

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <cassert>
#include <ctime>
#include <cmath>

using namespace std;

//#define NAMEOF(name) #name

constexpr int DAY_MAX = 100;
enum class Color : uint8_t{
    Red = 0,
    Purple = 1,
    White = 2,
    Blue = 3,
    Max = 4,
};
constexpr Color MAIN_COLORS[] = {
    Color::Red,
    Color::Purple,
    Color::White,
    Color::Blue,
};
unordered_map<Color, string> color_to_str = {
    {Color::Red, "Red"},
    {Color::Purple, "Purple"},
    {Color::White, "White"},
    {Color::Blue, "Blue"},
};
constexpr int REQUIRED[] = {
    100, 
    80, 
    50, 
    (int)1e6
};
constexpr inline int get_blue_upper_limit(){
    int blues_max = 0;
    for(int k = 1; k <= DAY_MAX; k++){
        int required[(int)Color::Max] = {};

        for(Color color : MAIN_COLORS){
            required[(int)color] = REQUIRED[(int)color];
        }
        required[(int)Color::Red] -= k - 1;
        required[(int)Color::Purple] -= DAY_MAX - (k - 1);
        required[(int)Color::White] -= DAY_MAX / 2 - (k - 1) / 2;
        for(Color color : MAIN_COLORS){
            required[(int)color] = max(required[(int)color], 0);
        }

        int cost = 0;
        for(Color color : MAIN_COLORS){
            if(color == Color::Blue) continue;
            const int p = required[(int)color] / 25;
            const int r = required[(int)color] % 25;
            cost += 25 * p * (p + 1) / 2 + (p + 1) * r;
        }
        int money = 5 * DAY_MAX - cost;
        int blues = 0;
        while(true){
            if(money >= 1 + blues / 25){
                money -= 1 + blues / 25;
                blues++;
            }
            else{
                break;
            }
        }
        blues += (k - 1) / 2;
        blues_max = max(blues, blues_max);
    }

    return blues_max;
}
constexpr int BLUE_UPPER_LIMIT = get_blue_upper_limit();

constexpr inline int get_we_bought(const Color color, const int i_bought, const int day, const int k){
    assert(0 <= (int)color && (int)color < (int)Color::Max);
    assert(i_bought >= 0);
    assert(0 <= day && day <= DAY_MAX);
    assert(1 <= k && k <= DAY_MAX);

    int we_bought = i_bought;
    switch (color)
    {
    case Color::Red:
        we_bought += std::min(day, k - 1);
        break;
    case Color::Blue:
        we_bought += std::min(day, k - 1) / 2;
        break;
    case Color::Purple:
        we_bought += day - std::min(day, k - 1);
        break;
    case Color::White:
        we_bought += day / 2 - std::min(day, k - 1) / 2;
        break;
    default:
        break;
    }
    return we_bought;
}

constexpr inline int get_total_sold(const Color color, const int i_bought, const int day, const int k){
    assert(0 <= (int)color && (int)color < (int)Color::Max);
    assert(i_bought >= 0);
    assert(0 <= day && day <= DAY_MAX);
    assert(1 <= k && k <= DAY_MAX);
    return get_we_bought(color, i_bought, day, k) + day / 5;
}

constexpr inline int get_price(const Color color, const int bought, const int day, const int k){
    assert(0 <= (int)color && (int)color < (int)Color::Max);
    assert(bought >= 0);
    assert(0 <= day && day <= DAY_MAX);
    assert(1 <= k && k <= DAY_MAX);
    return 1 + get_total_sold(color, bought, day, k) / 25;
}

// 安いものから順に買う
// 同じ値段なら赤→紫→白→青
int naive_solve(const int k, const bool print_details = false){
    vector<int> bought((int)Color::Max);
    int money = 0;
    for(int day = 1; day <= DAY_MAX; day++){
        money += 5;
        while(true){
            vector<int> costs((int)Color::Max);
            for(int i = 0; i < (int)Color::Max; i++){
                Color c = (Color)i;
                costs[i] = get_price(c, bought[i], day, k);
                if(get_we_bought(c, bought[i], day, k) >= REQUIRED[i]){ // 既に必要数買っている
                    costs[i] += 1e6;
                }
            }
            auto min_itr = min_element(costs.begin(), costs.end());
            if(money >= *min_itr){
                int min_index = distance(costs.begin(), min_itr);
                bought[min_index]++;
                money -= *min_itr;
            }
            else{
                break;
            }
        }
    }
    if(print_details){
        for(Color c : MAIN_COLORS){
            int i = (int)c;
            cout << color_to_str[c] << ", " << get_we_bought(c, bought[i], DAY_MAX, k) << endl;
        }
    }
    int blue_bought = get_we_bought(Color::Blue, bought[(int)Color::Blue], DAY_MAX, k);
    for(Color c : MAIN_COLORS){
        if(c == Color::Blue) continue;
        int i = (int)c;
        if(get_we_bought(c, bought[i], DAY_MAX, k) < REQUIRED[i]){
            blue_bought = -1;
            break;
        }
    }
    return blue_bought;
}

using dp_key_t = uint32_t;
using dp_indices_t = tuple<int, int, int, int>;
using dp_cost_t = uint16_t;
ostream& operator<<(ostream& os, const dp_indices_t &i) {
    array<int, (int)Color::Max> len;
    for(int i = 0; i < (int)Color::Max; i++){
        len[i] =  (int)log10(i != (int)Color::Blue ? REQUIRED[i] : BLUE_UPPER_LIMIT) + 1;
    }
    os << "{" 
    << color_to_str[(Color)0][0] << ": " << std::setw(len[0]) << get<0>(i) << ", " 
    << color_to_str[(Color)1][0] << ": " << std::setw(len[1]) << get<1>(i) << ", " 
    << color_to_str[(Color)2][0] << ": " << std::setw(len[2]) << get<2>(i) << ", " 
    << color_to_str[(Color)3][0] << ": " << std::setw(len[3]) << get<3>(i) 
    << "}" ;
    return os;
}

constexpr inline dp_key_t dp_key(const int r, const int p, const int w, const int b){
    constexpr int bits = 8;
    return
        ((dp_key_t)b << (bits * 3)) + 
        ((dp_key_t)w << (bits * 2)) + 
        ((dp_key_t)p << bits) + 
        (dp_key_t)r;
}

constexpr inline dp_key_t dp_key(const dp_indices_t i){
    constexpr int bits = 8;
    return dp_key(get<0>(i), get<1>(i), get<2>(i), get<3>(i));
}

constexpr inline dp_indices_t dp_indices(const dp_key_t key){
    constexpr int bits = 8;
    constexpr int mask = (1 << bits) - 1;
    return {
        key & mask,
        (key >> bits) & mask,
        (key >> (bits * 2)) & mask,
        (key >> (bits * 3))
    };
}

/*
constexpr inline dp_key_t get_we_boughts(const dp_key_t i_bought, const int day, const int k){
    assert(i_bought >= 0);
    assert(0 <= day && day <= DAY_MAX);
    assert(1 <= k && k <= DAY_MAX);

    constexpr int bits = 8;
    const int m = std::min(day, k - 1);
    return i_bought + 
        m +
        ((m / 2) << (bits * 3)) +
        ((day - m) << bits) +
        ((day / 2 - m / 2) << (bits * 2));
}

constexpr inline dp_key_t get_total_solds(const dp_key_t i_bought, const int day, const int k){
    assert(i_bought >= 0);
    assert(0 <= day && day <= DAY_MAX);
    assert(1 <= k && k <= DAY_MAX);

    constexpr int bits = 8;
    constexpr int added = (1 << (bits * 3)) + (1 << (bits * 2)) + (1 << bits) + 1; 

    return get_we_boughts(i_bought, day, k) + (day / 5) * added;
}
*/

int dp_solve(const int k, const bool print_details = false){
    unordered_map<dp_key_t, pair<dp_cost_t, Color>> dp;
    constexpr int R = REQUIRED[(int)Color::Red];
    constexpr int P = REQUIRED[(int)Color::Purple];
    constexpr int W = REQUIRED[(int)Color::White];
    constexpr int B = BLUE_UPPER_LIMIT;
    constexpr int D = DAY_MAX;
    dp[dp_key(0, 0, 0, 0)] = make_pair(0, Color::Max);
    int loop_count = 0, update_count = 0, filled_count = 0;
    while(true){
        loop_count++;
        bool updated = false;
        for(auto it = dp.begin(); it != dp.end(); ++it){
            int r, p, w, b;
            tie(r, p, w, b) = dp_indices(it->first);
            array<int, (int)Color::Max> bought = {r, p, w, b};
            const dp_cost_t current_cost = (it->second).first;
            const int current_day = current_cost > 0 ? ((current_cost - 1) / 5 + 1) : 0;
            for(Color color : MAIN_COLORS){
                // 既に購入数を満たしている場合は無視
                if(get_we_bought(color, bought[(int)color], current_day, k) >= REQUIRED[(int)color]){
                    continue;
                }

                // 日付を進める
                int day = current_day;
                while(true){
                    const int price = get_price(color, bought[(int)color], day, k);
                    const int money = 5 * day - current_cost;
                    if(price <= money){
                        break;
                    }
                    else{
                        const int lack = price - money;
                        day += (lack - 1) / 5 + 1;
                    }
                }

                // 日付が限界を迎えた場合は無視
                if(day > DAY_MAX) continue;

                // コストを計算
                dp_cost_t cost = get_price(color, bought[(int)color], day, k);
                dp_cost_t new_cost = current_cost + cost;
                
                // キーを計算
                int new_key = 0;
                array<int, (int)Color::Max> i_bought = bought;
                for(Color color : MAIN_COLORS){
                    int friend_bought = get_we_bought(color, 0, current_day, k);
                    i_bought[(int)color] -= friend_bought;
                }
                array<int, (int)Color::Max> new_i_bought = i_bought;
                new_i_bought[(int)color]++;
                for(Color color : MAIN_COLORS){
                    int t = get_we_bought(color, new_i_bought[(int)color], day, k);
                    t = min(REQUIRED[(int)color], t);
                    new_key += t << (8 * (int)color);
                }

                // 更新
                if(!dp.contains(new_key) || new_cost < dp[new_key].first){
                    if(!dp.contains(new_key)) filled_count++;
                    update_count++;
                    dp[new_key] = make_pair(new_cost, color);
                    updated = true;
                    if(print_details){
                        stringstream ss;
                        ss << dp_indices(new_key);
                        printf("update: %s, cost = %3d, new_cost = %3d, day = %3d, %s\n", 
                            ss.str().c_str(), cost, new_cost, day, color_to_str[color].c_str());
                    }
                }
            }
        }
        printf("loops = %d, updated = %d, filled_count = %d\n", loop_count, update_count, filled_count);
        if(!updated) break;
    }
    
    for(int b = B; b >= 0; b--){
        if(dp.contains(dp_key(R, P, W, b))){
            dp_key_t key = dp_key(R, P, W, b);
            // 履歴計算
            return b;
        }
    }
    return -1;
}

int main(){
    //for(int k = 1; k <= DAY_MAX; k++){
    for(int k = DAY_MAX; k <= DAY_MAX; k++){
        clock_t t, t1, t2;
        t = clock();
        int naive_answer = naive_solve(k, false);
        t1 = clock() - t;
        t = clock();
        int full_days_dp_answer = dp_solve(k, false);
        t2 = clock() - t;
        cout
        << k << ": " 
        << naive_answer << " (" << ((double)t1 / CLOCKS_PER_SEC) << " sec)"
        << ", " 
        << full_days_dp_answer << " (" << ((double)t2 / CLOCKS_PER_SEC) << " sec)"
        << endl;
    }
}