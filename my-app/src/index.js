import React from 'react';
import ReactDOM from 'react-dom';
import Websocket from 'react-websocket';

import './index.css';

const client=new WebSocket('ws://localhost:9001');
client.onopen = () => {
    		console.log('open connection');
}
function Square(props) {
    return (
      <button className="square" onClick={props.onClick}>
        {props.value}
      </button>
    );
  }  
function OnOff(props){
  return(
    <button  className="OnOff" onClick={props.onClick}>
    {props.value}
    </button>
  )
}
  
  class Board extends React.Component {
    constructor(props) {   
         super(props);    
         this.state = {      
             squares: Array(9).fill(null),    
             xIsNext:true,
            GameStep:0,
            On_Off:false,
	    OnDis:'begin',
        }; 
    }    
    handleData(data){
	let result = JSON.parse(data);
	this.setState({count: this.state.count + result.movement});
    }
    handleClick(i){
        const squares=this.state.squares.slice();
	const OnState=this.state.On_Off;
	const Steps=this.state.GameStep;
	
	if(!OnState){
	    return;
	}
        if (calculateWinner(squares) || squares[i]) {      
            return; 
        }
        if(Steps===9){
            return; 
        }
        squares[i] = this.state.xIsNext ? 'X' : 'O';
	if(Steps===8){
	  this.setState({
            squares:squares,
            xIsNext:!this.state.xIsNext,
	    GameStep:Steps+1
            }
	  );
	}
	else{
	  client.send(JSON.stringify({
   		username: this.state.xIsNext,
   		content: squares
 		}));
	  let curNext=true;
	  client.onmessage = event => {
    		console.log(event.data);
		let newDataJson= JSON.parse(event.data);
		curNext=newDataJson["username"];
		let newsquares=newDataJson["content"];
		for(let i=0;i<9;++i){
		    squares[i]=newsquares[i];
		}

	  }
          this.setState({
            squares:squares,
            xIsNext:curNext,
	    GameStep:Steps+2
            }
	  );
	}
        
    }
    handleOnClick(c){
	const OnState=this.state.On_Off;
	const squares=this.state.squares.slice();
	if(OnState){//结束
	  for(let i=0;i<9;++i){
	     squares[i]=null;
	  }
	  client.onclose = () => {
        	console.log('Close connected')
    	  }
	  this.setState({
            squares:squares,
            xIsNext:true,
	    GameStep:0,
	    On_Off:false,
	    OnDis:'begin',
          });	
	}
	else{//开始
		client.onopen = () => {
    		console.log('open connection');
		}
	    this.setState({
	      On_Off:true,
	      OnDis:'end',
            });
	}
    }
    renderSquare(i) {
      return <Square  
            value={this.state.squares[i]}
            onClick={()=>this.handleClick(i)}
        />;
    }
    renderOnOff(c){
      return <OnOff  
            value={this.state.OnDis}
            onClick={()=>this.handleOnClick(c)}
        />;
    }
    componentDidMount() {
	this.timerID = setInterval(
	  () => this.tick(),
      	  100
	);
    }
   tick() {
	this.setState({
        squares:this.state.squares,
        });
   }
	
    render() {
        const winner = calculateWinner(this.state.squares);
	const OnState=this.state.OnOff;    
        let status; 
        if (winner) {      
            status = 'Winner: ' + winner;    
        } 
        else if(this.state.GameStep===9){
            status = 'It is a draw! ' ;    
        }
        else {  
            status = 'Next player: ' + (this.state.xIsNext ? 'X' : 'O');    
        }
	
  
      return (
        <div>
          <div className="status">{status}</div>
          <div className="board-row">
	    {this.renderSquare(0)}
            {this.renderSquare(1)}
            {this.renderSquare(2)}
          </div>
          <div className="board-row">
            {this.renderSquare(3)}
            {this.renderSquare(4)}
            {this.renderSquare(5)}
          </div>
          <div className="board-row">
            {this.renderSquare(6)}
            {this.renderSquare(7)}
            {this.renderSquare(8)}
          </div>
	  <div className="OnOff" onclick>
	    {this.renderOnOff(false)}
	  </div>
        </div>
	
      );
    }
  }
  
  class Game extends React.Component {
    render() {
      return (
        <div className="game">
          <div className="game-board">
            <Board />
          </div>
          <div className="game-info">
            <div>{/* status */}</div>
            <ol>{/* TODO */}</ol>
          </div>
        </div>
      );
    }
  }
  
  // ========================================
  
  ReactDOM.render(
    <Game />,
    document.getElementById('root')
  );
  function calculateWinner(squares) {
    const lines = [
      [0, 1, 2],
      [3, 4, 5],
      [6, 7, 8],
      [0, 3, 6],
      [1, 4, 7],
      [2, 5, 8],
      [0, 4, 8],
      [2, 4, 6],
    ];
    for (let i = 0; i < lines.length; i++) {
      const [a, b, c] = lines[i];
      if (squares[a] && squares[a] === squares[b] && squares[a] === squares[c]) {
        return squares[a];
      }
    }
    return null;
  }
