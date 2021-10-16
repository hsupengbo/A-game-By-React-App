#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <functional>
#include <iostream>
#include <nlohmann/json.hpp>


typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using namespace std;
using json= nlohmann::json;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;
typedef nlohmann::detail::value_t Value_T;

class utility_server {
public:
    utility_server() {
        m_endpoint.set_reuse_addr(true);
        // Set logging settings
        m_endpoint.set_error_channels(websocketpp::log::elevel::all);
        m_endpoint.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);
        // Initialize Asio
        m_endpoint.init_asio();
        // Register our message handler
        m_endpoint.set_message_handler(std::bind(
                &utility_server::echo_handler, this,
                std::placeholders::_1, std::placeholders::_2
        ));
    }
    void ComputerMove(){
        memset(A,0,sizeof(A));
        for (int i=0;i<3;i++) {
            for (int j=0;j<3;j++) {
                if (board["content"][i*3+j]=="X")
                    A[i][j]='X';
                else if (board["content"][i*3+j]=="O")
                    A[i][j]='O';
            }
        }
        int x=0,y=0,score=1;
        cout<<score<<endl;
        for (int i=0;i<3;i++) {
            for (int j=0;j<3;j++) {
                if (!A[i][j]) {
                    A[i][j]='O';
                    int temp=MaxSearch();
                    cout<<"temp:"<<temp<<endl;
                    if (score>temp){
                        x=i;y=j;score=temp;
                    }
                    A[i][j]=0;
                }
            }
        }
        cout<<"x:"<<x<<",y:"<<y<<endl;
        board["content"][x*3+y]="O";
    }
    int MaxSearch(){
        int ret=-1,sta=checkWin();
        if (sta) return sta;
        if (isFull()) return 0;
        for (int i=0;i<3;i++){
            for (int j=0;j<3;j++)
                if (!A[i][j]) {
                    A[i][j]='X';
                    ret=max(ret,MinSearch());
                    A[i][j]=0;
                }
        }
        return ret;
    }
    int MinSearch(){
        int ret=1,sta=checkWin();
        if (sta) return sta;
        if (isFull()) return 0;
        for (int i=0;i<3;i++){
            for (int j=0;j<3;j++){
                if (!A[i][j]) {
                    A[i][j]='O';
                    ret=min(ret,MaxSearch());
                    A[i][j]=0;
                }
            }
        }
        return ret;
    }
    bool isFull(){
        for (int i=0;i<3;i++)
            for (int j=0;j<3;j++)
                if (!A[i][j]) return false;
        return true;
    }
    int checkWin() {
        for (int i=0;i<3;i++) if (A[i][0]=='O' && A[i][1]=='O' && A[i][2]=='O') return -1;
        for (int i=0;i<3;i++) if (A[0][i]=='O' && A[1][i]=='O' && A[2][i]=='O') return -1;
        for (int i=0;i<3;i++)  if (A[i][0]=='X' && A[i][1]=='X' && A[i][2]=='X') return 1;
        for (int i=0;i<3;i++) if (A[0][i]=='X' && A[1][i]=='X' && A[2][i]=='X') return 1;
        if (A[0][0]=='O' && A[1][1]=='O' && A[2][2]=='O') return -1;
        if (A[2][0]=='O' && A[1][1]=='O' && A[0][2]=='O') return -1;
        if (A[0][0]=='X' && A[1][1]=='X' && A[2][2]=='X') return 1;
        if (A[2][0]=='X' && A[1][1]=='X' && A[0][2]=='X') return 1;
        return 0;
    }
    void handleGame(){
        cout<<"XisNext: "<<board["username"]<<endl;
        if(board["username"]){
            ComputerMove();
        }
        cout<<"get!"<<endl;
    }
    void echo_handler(websocketpp::connection_hdl hdl, server::message_ptr msg) {
        std::cout << "收到"
                  << ", message: " << msg->get_payload()
                  << std::endl;
        board = json::parse(msg->get_payload());

        if (msg->get_payload() == "stop-listening") {
            m_endpoint.stop_listening();
            return;
        }
        handleGame();
        // write a new message
        try {
            msg->set_payload(to_string(board));
            m_endpoint.send(hdl, msg->get_payload() , msg->get_opcode());
        } catch (websocketpp::exception const & e) {
            std::cout << "Echo failed because: "
                      << "(" << e.what() << ")" << std::endl;
        }
    }

    void run() {
        try{
            // Listen on port 9002
            m_endpoint.listen(9001);
            // Queues a connection accept operation
            m_endpoint.start_accept();
            // Start the Asio io_service run loop
            m_endpoint.run();
        }
        catch (websocketpp::exception const & e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "other exception" << std::endl;
        }
    }
private:
    server m_endpoint;
    json board;
    char A[3][3];
};

int main() {

    utility_server s;
    s.run();
    return 0;
}
