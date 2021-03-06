import '../config';
import { Link } from 'react-router-dom';
import $ from 'jquery';
import React, { Component } from 'react';
//import './detail.css';
import { message, Button, Card, Modal, Tag, Tabs, List} from 'antd';
import Loading from '../loading.js'
import TeamList from '../team/list.js';
import TeamAdmin from '../team/admin.js';
import BlogList from '../blog/list.js';
import ProblemList from '../problem/list.js';
import SubmissionList from '../submission/list.js';
import ContestLeaderboard from './leaderboard.js';
import MarkdownView from '../markdown/view.js'
const { confirm } = Modal;
const { TabPane } = Tabs;

class Detail extends Component{
	state = {
	}
	changeTabKey = (key) => {
		const { id } = this.props.match.params;
		this.props.history.push('/contest/' + id + '/' + key);
	}
	getInfo = (id = this.props.match.params.id) => {
		let url = global.constants.server + 'contest/'
		$.get({
			url: url,
			crossDomain: true,
			data: {'id': id},
			xhrFields: {
				withCredentials: true
			},
			async: true,
			success: function (result) {
				this.setState({data: result})
			}.bind(this),
			error: function (result) {
				message.error(result.responseText)
			}.bind(this),
		})
	}
	componentWillMount(){
		this.getInfo()
	}
	componentWillReceiveProps(nextProps){
		if (this.props.match.params.id != nextProps.match.params.id){
			this.getInfo(nextProps.match.params.id);
		}
	}
	tabChage = (key) => {
		this.changeTabKey(key)
	}
	render(){
		if (this.state.data == null){
			return (
				<Loading/>
			)
		}
		const { user } = this.props;
		const { id, tab } = this.props.match.params;
		let contest = this.state.data;
		let key = 'home';
		if (tab) key = tab;
		return (
			<div className = "root">
				<div className='title'> {contest.name} </div>
				<Tabs activeKey={key} onTabClick={this.tabChage}>
					<TabPane tab="Home" key="home">
						<div style={{marginLeft: 10}}>
							<MarkdownView
								source={contest.detail}
							/>
						</div>
					</TabPane>
					
					{contest.problems.length>0 && <TabPane tab="Problem List" key="problemlist">
						<ProblemList
							contestId={id}
							going={contest.going}
							padding={30}
						/>
					</TabPane>}
					{contest.problems.length>0 && <TabPane tab="Leaderboard" key="leaderboard">
						<ContestLeaderboard
							contestId={id}
							problems={contest.problems}
							padding={10}
						/>
					</TabPane>}
					{contest.problems.length>0 && <TabPane tab="Submission List" key="submission">
						<SubmissionList
							contestId={id}
							padding={10}
						/>
					</TabPane>}
					<TabPane tab="Blogs" key="blog">
						<BlogList
							user={this.props.user}
							padding={10}
							tag={contest.name}
							author='ASTA'
							{...this.props}
						/>
					</TabPane>
					<TabPane tab="My Team" key="admin">
						<TeamAdmin
							changeTabKey={this.changeTabKey}
							contestId={id}
							padding={10}
							{...this.props}
						/>
					</TabPane>

					<TabPane tab="Team List" key="team">
						<TeamList
							user={this.props.user}
							contestId={id}
							padding={10}
							{...this.props}
						/>
					</TabPane>
				</Tabs>
			</div>
		)
	}
}

export default Detail;