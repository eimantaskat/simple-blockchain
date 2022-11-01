#include "../include/blockchain.hpp"

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

    int sending_value = 0;
    int inputs_amount = 0;
    int outputs_amount = 0;
    for (auto tx_it = transactions.begin(); tx_it != transactions.end(); ++tx_it) {
        sending_value += tx_it->amount;
        inputs_amount += tx_it->in.size();
        outputs_amount += tx_it->out.size();
    }

    int block_height = b.height;
    std::string block_hash = b.hash;
    float block_capacity = capacity_percentage;
    std::string block_distance = time_distance_to_str(time_distance);
    std::string block_mined_on = epoch_to_date(b.time);
    int block_sending_value = sending_value;
    int block_inputs_amount = inputs_amount;
    int block_outputs_amount = outputs_amount;
    std::string block_version = b.version;
    std::string block_merkleroot = b.merkleroot;
    int block_difficulity = b.difficulity_target;
    unsigned long long block_nonce = b.nonce;

    // generate HTML
    std::stringstream html;

    html << "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><style>body { font-family: Sans-Serif; } .grid { display: grid; grid-template-columns: 5fr 3fr; grid-template-rows: 180px 2fr 2fr; grid-column-gap: 0px; grid-row-gap: 0px; } .header { grid-area: 1 / 1 / 2 / 2; display: flex; padding: 4%; border-bottom: 1px solid rgb(238, 238, 238); } .header h1 { font-weight: 600; margin-left: 5%; } .info { grid-area: 2 / 1 / 4 / 2; padding: 4%; } .tx { grid-area: 1 / 2 / 4 / 3; padding: 4%; border-left: 1px solid rgb(238, 238, 238); min-height: 100%; } .block { position: relative; width: 8vh; height: 8vh; background: rgb(2,0,36); border: 1px solid black; border-radius: 10px; } table { width: 100%; } table th { padding:21px 25px 22px 25px; border-top:1px solid #fafafa; border-bottom:1px solid #e0e0e0; background: #ededed; } table tr { text-align: center; padding-left:20px; } table td:first-child { text-align: left; padding-left:20px; border-left: 0; width: 40%; } table td { padding:10px; border-top: 1px solid #ffffff; border-bottom:1px solid #e0e0e0; border-left: 1px solid #e0e0e0; } table tr.even td { background: #f6f6f6; } .transaction { border: 1px solid #e0e0e0; text-align: center; margin-bottom: 10px; } .transaction h5 { font-weight: 400; }</style><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><script src=\"https://kit.fontawesome.com/36b2e7af05.js\" crossorigin=\"anonymous\"></script><title>Block #"
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
                int tx_amount = tx_it->amount;
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

void Blockchain::generate_transaction_html(transaction tx) {
    // collect and calculate data
    std::string tx_hash = tx.id;

    std::string tx_status;
    if (cached_transactions.find(tx_hash) != cached_transactions.end()) {
        tx_status = "Confirmed";
    } else {
        tx_status = "Unconfirmed";
    }

    std::string tx_from = tx.from;
    std::string tx_to = tx.to;
    std::string tx_time = epoch_to_date(tx.time);
    auto tx_inputs = tx.in;
    auto tx_outputs = tx.out;

    int tx_in_amount = 0;
    for (auto txo_it = tx_inputs.begin(); txo_it != tx_inputs.end(); ++txo_it) {
        tx_in_amount += txo_it->amount;
    }

    int tx_out_amount = 0;
    for (auto txo_it = tx_outputs.begin(); txo_it != tx_outputs.end(); ++txo_it) {
        tx_out_amount += txo_it->amount;
    }

    int tx_amount_sent = tx.amount;

    int tx_block = -1;
    for (int i = 0; i < blockchain_height; ++i) {
        block b = read_block(i);
        
        if (std::count(b.tx.begin(), b.tx.end(), tx_hash)) {
            tx_block = i;
            break;
        }
    }

    // generate HTML
    std::stringstream html;

    html << "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><style>body{font-family:Sans-Serif}.grid{display:grid;grid-template-columns:5fr 3fr;grid-template-rows:200px 2fr 2fr;grid-column-gap:0;grid-row-gap:0}.header{grid-area:1/1/2/2;display:block;padding:4%;border-bottom:1px solid #eee;text-align:center}.header h2{font-weight:600;margin-left:5%}.header h3{font-weight:400}.info{grid-area:2/1/4/2;padding:4%}.txos{grid-area:1/2/4/3;padding:4%;border-left:1px solid #eee;min-height:100%}.block{position:relative;width:8vh;height:8vh;background:#020024;border:1px solid #000;border-radius:10px}table{width:100%}table th{padding:21px 25px 22px 25px;border-top:1px solid #fafafa;border-bottom:1px solid #e0e0e0;background:#ededed}table tr{text-align:center;padding-left:20px}table td:first-child{text-align:left;padding-left:20px;border-left:0;width:40%}table td{padding:10px;border-top:1px solid #fff;border-bottom:1px solid #e0e0e0;border-left:1px solid #e0e0e0}table tr.even td{background:#f6f6f6}.transaction{border:1px solid #e0e0e0;text-align:center;margin-bottom:10px}.transaction h5{font-weight:400}</style><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><script src=\"https://kit.fontawesome.com/36b2e7af05.js\" crossorigin=\"anonymous\"></script><title>Transaction</title></head><body><div class=\"grid\"><div class=\"header\"><h2>Transaction "
            << tx_hash
            << "</h2></div><div class=\"info\"><h2>Details</h2><div class=\"table\"><table cellspacing=\"0\"><tbody><tr><td>Hash</td><td>"
            << tx_hash
            << "</td></tr><tr class=\"even\"><td>Status</td><td>"
            << tx_status
            << "</td></tr><tr><td>From</td><td>"
            << tx_from
            << "</td></tr><tr class=\"even\"><td>To</td><td>"
            << tx_to
            << "</td></tr><tr><td>Received Time</td><td>"
            << tx_time
            << "</td></tr><tr class=\"even\"><td>Total Input</td><td>"
            << tx_in_amount
            << "</td></tr><tr><td>Total Output</td><td>"
            << tx_out_amount
            << "</td></tr><tr class=\"even\"><td>Amount sent</td><td>"
            << tx_amount_sent
            << "</td></tr><tr><td style=\"border-bottom:none\">Included in Block</td><td style=\"border-bottom:none\">"
            << tx_block
            << "</td></tr></tbody></table></div></div><div class=\"txos\"><h2>Inputs</h2>";

            for (auto txo_it = tx_inputs.begin(); txo_it != tx_inputs.end(); ++txo_it) {
                std::string txo_to = txo_it->to;
                int txo_amount = txo_it->amount;

                std::string txo_details;
                if (txo_it->unspent) {
                    txo_details = "Unspent";
                } else {
                    txo_details = "Output";
                }

                html << "<div class=\"transaction\"><h5>Index: {index}<br><br><i class=\"fa-solid fa-arrow-right\"></i> "
                        << txo_to
                        << "<br><br>"
                        << txo_amount
                        << " <i class=\"fa-sharp fa-solid fa-coins\"></i><br><br>Details: "
                        << txo_details
                        << "</h5></div>";
            }

    html << "<h2>Outputs</h2>";

            for (auto txo_it = tx_outputs.begin(); txo_it != tx_outputs.end(); ++txo_it) {
                std::string txo_to = txo_it->to;
                int txo_amount = txo_it->amount;
                
                std::string txo_details;
                if (txo_it->unspent) {
                    txo_details = "Unspent";
                } else {
                    txo_details = "Output";
                }

                html << "<div class=\"transaction\"><h5>Index: {index}<br><br><i class=\"fa-solid fa-arrow-right\"></i> "
                        << txo_to
                        << "<br><br>"
                        << txo_amount
                        << " <i class=\"fa-sharp fa-solid fa-coins\"></i><br><br>Details: "
                        << txo_details
                        << "</h5></div>";
            }
    html << "</div></div></body></html>";

    // write to file
    std::ofstream file(html_transaction_file);
    file << html.rdbuf();
    std::cout << "Transaction HTML file generated: " << html_transaction_file << "\n";
}