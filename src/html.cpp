#include "../include/blockchain.hpp"

std::string epoch_to_date(long time) {
    const time_t t = (time_t)time;
    return ctime(&t);
}

std::string time_distance_to_str(long distance) {
    int days = distance / 60 / 60 / 24;
    int hours = (distance / 60 / 60) % 24;
    int minutes = (distance / 60) % 60;
    int seconds = distance % 60;

    std::string str_distance = std::to_string(days) + "d " + std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
    return str_distance;
}

void Blockchain::generate_block_html(block b) {
    // collect and calculate data
    int tx_count = b.tx.size();
    float capacity_percentage = (float)tx_count / max_block_tx * 100;
    long time_distance = get_epoch_time() - b.time;

    std::vector<transaction> transactions;
    for (auto tx_it = b.tx.begin(); tx_it != b.tx.end(); ++tx_it) {
        auto pair = cached_transactions.find(*tx_it);
        transaction tx = pair->second;
        transactions.push_back(tx);
    }

    double sending_value;
    int inputs_amount;
    int outputs_amount;
    for (auto tx_it = transactions.begin(); tx_it != transactions.end(); ++tx_it) {
        sending_value += tx_it->amount;
        inputs_amount += tx_it->in.size();
        outputs_amount += tx_it->out.size();
    }

    std::string block_height = std::to_string(b.height);
    std::string block_hash = b.hash;
    std::string block_capacity = std::to_string(capacity_percentage);
    std::string block_distance = time_distance_to_str(time_distance);
    std::string block_mined_on = epoch_to_date(b.time);
    std::string block_sending_value = std::to_string(sending_value);
    std::string block_inputs_amount = std::to_string(inputs_amount);
    std::string block_outputs_amount = std::to_string(outputs_amount);
    std::string block_version = b.version;
    std::string block_merkleroot = b.merkleroot;
    std::string block_difficulity = std::to_string(b.difficulity_target);
    std::string block_nonce = std::to_string(b.nonce);

    // generate HTML
    std::stringstream html;

    html << "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><link rel=\"stylesheet\" href=\"./html/main.css\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><script src=\"https://kit.fontawesome.com/36b2e7af05.js\" crossorigin=\"anonymous\"></script><title>Block #"
            << block_height
            << "</title></head><body><div class=\"grid\"><div class=\"header\"><div class=\"block\" style=\"background:linear-gradient(0deg,rgba(13,0,236,1) 0,rgba(13,0,236,1) "
            << block_capacity << "%"
            << ",rgba(255,255,255,1) "
            << block_capacity << "%"
            << ")\"></div><h1>Block #"
            << block_height
            << "</h1></div><div class=\"info\"><h2>Details</h2><table cellspacing=\"0\"><tbody><tr><td>Hash</td><td>"
            << block_hash
            << "</td></tr><tr class=\"even\"><td>Capacity</td><td>"
            << block_capacity << "%"
            << "</td></tr><tr><td>Distance</td><td>"
            << block_distance
            << "</td></tr><tr class=\"even\"><td>Mined on</td><td>"
            << block_mined_on
            << "</td></tr><tr><td>Transactions</td><td>"
            << tx_count
            << "</td></tr><tr class=\"even\"><td>Value</td><td>"
            << block_sending_value
            << "</td></tr><tr><td>Inputs</td><td>"
            << block_inputs_amount
            << "</td></tr><tr class=\"even\"><td>Outputs</td><td>"
            << block_outputs_amount
            << "</td></tr><tr><td>Version</td><td>"
            << block_version
            << "</td></tr><tr class=\"even\"><td>Merkle Root</td><td>"
            << block_merkleroot
            << "</td></tr><tr><td>Difficulity</td><td>"
            << block_difficulity
            << "</td></tr><tr class=\"even\"><td>Nonce</td><td>"
            << block_nonce
            << "</td></tr><tr><td style=\"border-bottom:none\">Height</td><td style=\"border-bottom:none\">"
            << block_height
            << "</td></tr></tbody></table></div><div class=\"tx\"><h2>Transactions</h2>";

            for (auto tx_it = transactions.begin(); tx_it != transactions.end(); ++tx_it) {
                std::string tx_id = tx_it->id;
                std::string tx_from = tx_it->from;
                std::string tx_to = tx_it->to;
                double tx_amount = tx_it->amount;
                html << "<div class=\"transaction\"><h4>"
                        << tx_id
                        << "</h4><h5>"
                        << tx_from
                        << "<br><i class=\"fa-solid fa-arrow-down\"></i><br>"
                        << tx_to
                        << "<br><br>"
                        << tx_amount
                        << " <i class=\"fa-sharp fa-solid fa-coins\"></i></h5></div>";
            }


    html << "</div></div></body></html>";

    // write to file
    std::ofstream file(html_block_file);
    file << html.rdbuf();
    std::cout << "Block HTML file generated: " << html_block_file << "\n";
}