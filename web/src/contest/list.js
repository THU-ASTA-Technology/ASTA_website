import '../config';
import { Link } from 'react-router-dom';
import $ from 'jquery';
import React, { Component } from 'react';
import Loading from '../loading.js'
import { Button, Card, Col, Row, Collapse } from 'antd';
const { Panel } = Collapse;


class ListElement extends Component{
	state = {
	}
	getList = () => {
		let url = global.constants.server + 'contest/list/';
		this.contestListRequest = $.get({
			url: url,
			success: function (result) {
				this.setState({list : result});
			}.bind(this)
		})
	}
	componentWillMount(){
		this.getList();
	}
	render(){
		if (this.state.list == null){
			return (
				<Loading/>
			)
		}
		return (
			<div className = "root" style = {{padding: 60}}>
				<h1 style={{lineHeight: 4, fontSize: 20,fontWeight: 700}}>Contests</h1>
				<Collapse accordion>
					{
						this.state.list.map((item, index) => {
							let title = item.name
							return (
								<Panel header={(
									<div>
										{item.name}
										&emsp;
										&emsp;
										<Link to={"/contest/" + item.id + "/home"} style={{display: 'inline'}}>
											<Button style={{display: 'inline'}}>Enter</Button>
										</Link>
										{item.register?(
											<span style={{color:'#3A5FCD'}}>
												报名进行中
											</span>
										):item.going?(
											<span style={{color:'red'}}>
												比赛进行中
											</span>
										):(
											<span style={{color:'grey'}}>
												比赛已结束
											</span>
										)}
									</div>
								)} key={index}>
									<p>{item.introduction}</p>
									<Link to={"/contest/" + item.id + "/home"} style={{display: 'inline'}}>
										<Button style={{display: 'inline'}}>Enter</Button>
									</Link>
								</Panel>
							)
						})
					}
				</Collapse>
			</div>
		)
	}
}
export default ListElement;
