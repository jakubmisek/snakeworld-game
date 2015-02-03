﻿#pragma warning disable 1591
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.3074
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace SnakeWorld_Server
{
	using System.Data.Linq;
	using System.Data.Linq.Mapping;
	using System.Data;
	using System.Collections.Generic;
	using System.Reflection;
	using System.Linq;
	using System.Linq.Expressions;
	using System.ComponentModel;
	using System;
	
	
	[System.Data.Linq.Mapping.DatabaseAttribute(Name="snakeworld")]
	public partial class snakeworldDataContext : System.Data.Linq.DataContext
	{
		
		private static System.Data.Linq.Mapping.MappingSource mappingSource = new AttributeMappingSource();
		
    #region Extensibility Method Definitions
    partial void OnCreated();
    #endregion
		
		public snakeworldDataContext() : 
				base(global::SnakeWorld_Server.Properties.Settings.Default.snakeworldConnectionString, mappingSource)
		{
			OnCreated();
		}
		
		public snakeworldDataContext(string connection) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public snakeworldDataContext(System.Data.IDbConnection connection) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public snakeworldDataContext(string connection, System.Data.Linq.Mapping.MappingSource mappingSource) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public snakeworldDataContext(System.Data.IDbConnection connection, System.Data.Linq.Mapping.MappingSource mappingSource) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public System.Data.Linq.Table<SnakeInfo> SnakeInfos
		{
			get
			{
				return this.GetTable<SnakeInfo>();
			}
		}
	}
	
	[Table(Name="dbo.SnakeInfo")]
	public partial class SnakeInfo
	{
		
		private int _userId;
		
		private int _timeSecondsPlayed;
		
		private double _maxLength;
		
		private int _kills;
		
		private int _suicides;
		
		private int _plays;
		
		private System.DateTime _playDate;
		
		public SnakeInfo()
		{
		}
		
		[Column(Storage="_userId", DbType="Int NOT NULL")]
		public int userId
		{
			get
			{
				return this._userId;
			}
			set
			{
				if ((this._userId != value))
				{
					this._userId = value;
				}
			}
		}
		
		[Column(Storage="_timeSecondsPlayed", DbType="Int NOT NULL")]
		public int timeSecondsPlayed
		{
			get
			{
				return this._timeSecondsPlayed;
			}
			set
			{
				if ((this._timeSecondsPlayed != value))
				{
					this._timeSecondsPlayed = value;
				}
			}
		}
		
		[Column(Storage="_maxLength", DbType="Float NOT NULL")]
		public double maxLength
		{
			get
			{
				return this._maxLength;
			}
			set
			{
				if ((this._maxLength != value))
				{
					this._maxLength = value;
				}
			}
		}
		
		[Column(Storage="_kills", DbType="Int NOT NULL")]
		public int kills
		{
			get
			{
				return this._kills;
			}
			set
			{
				if ((this._kills != value))
				{
					this._kills = value;
				}
			}
		}
		
		[Column(Storage="_suicides", DbType="Int NOT NULL")]
		public int suicides
		{
			get
			{
				return this._suicides;
			}
			set
			{
				if ((this._suicides != value))
				{
					this._suicides = value;
				}
			}
		}
		
		[Column(Storage="_plays", DbType="Int NOT NULL")]
		public int plays
		{
			get
			{
				return this._plays;
			}
			set
			{
				if ((this._plays != value))
				{
					this._plays = value;
				}
			}
		}
		
		[Column(Storage="_playDate", DbType="DateTime NOT NULL")]
		public System.DateTime playDate
		{
			get
			{
				return this._playDate;
			}
			set
			{
				if ((this._playDate != value))
				{
					this._playDate = value;
				}
			}
		}
	}
}
#pragma warning restore 1591
