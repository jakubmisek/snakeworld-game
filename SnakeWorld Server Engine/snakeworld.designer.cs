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
    partial void InsertSnakeInfo(SnakeInfo instance);
    partial void UpdateSnakeInfo(SnakeInfo instance);
    partial void DeleteSnakeInfo(SnakeInfo instance);
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
	public partial class SnakeInfo : INotifyPropertyChanging, INotifyPropertyChanged
	{
		
		private static PropertyChangingEventArgs emptyChangingEventArgs = new PropertyChangingEventArgs(String.Empty);
		
		private int _userId;
		
		private int _timeSecondsPlayed;
		
		private double _maxLength;
		
		private int _kills;
		
		private int _suicides;
		
		private int _plays;
		
    #region Extensibility Method Definitions
    partial void OnLoaded();
    partial void OnValidate(System.Data.Linq.ChangeAction action);
    partial void OnCreated();
    partial void OnuserIdChanging(int value);
    partial void OnuserIdChanged();
    partial void OntimeSecondsPlayedChanging(int value);
    partial void OntimeSecondsPlayedChanged();
    partial void OnmaxLengthChanging(double value);
    partial void OnmaxLengthChanged();
    partial void OnkillsChanging(int value);
    partial void OnkillsChanged();
    partial void OnsuicidesChanging(int value);
    partial void OnsuicidesChanged();
    partial void OnplaysChanging(int value);
    partial void OnplaysChanged();
    #endregion
		
		public SnakeInfo()
		{
			OnCreated();
		}
		
		[Column(Storage="_userId", DbType="Int NOT NULL", IsPrimaryKey=true)]
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
					this.OnuserIdChanging(value);
					this.SendPropertyChanging();
					this._userId = value;
					this.SendPropertyChanged("userId");
					this.OnuserIdChanged();
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
					this.OntimeSecondsPlayedChanging(value);
					this.SendPropertyChanging();
					this._timeSecondsPlayed = value;
					this.SendPropertyChanged("timeSecondsPlayed");
					this.OntimeSecondsPlayedChanged();
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
					this.OnmaxLengthChanging(value);
					this.SendPropertyChanging();
					this._maxLength = value;
					this.SendPropertyChanged("maxLength");
					this.OnmaxLengthChanged();
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
					this.OnkillsChanging(value);
					this.SendPropertyChanging();
					this._kills = value;
					this.SendPropertyChanged("kills");
					this.OnkillsChanged();
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
					this.OnsuicidesChanging(value);
					this.SendPropertyChanging();
					this._suicides = value;
					this.SendPropertyChanged("suicides");
					this.OnsuicidesChanged();
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
					this.OnplaysChanging(value);
					this.SendPropertyChanging();
					this._plays = value;
					this.SendPropertyChanged("plays");
					this.OnplaysChanged();
				}
			}
		}
		
		public event PropertyChangingEventHandler PropertyChanging;
		
		public event PropertyChangedEventHandler PropertyChanged;
		
		protected virtual void SendPropertyChanging()
		{
			if ((this.PropertyChanging != null))
			{
				this.PropertyChanging(this, emptyChangingEventArgs);
			}
		}
		
		protected virtual void SendPropertyChanged(String propertyName)
		{
			if ((this.PropertyChanged != null))
			{
				this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
			}
		}
	}
}
#pragma warning restore 1591
